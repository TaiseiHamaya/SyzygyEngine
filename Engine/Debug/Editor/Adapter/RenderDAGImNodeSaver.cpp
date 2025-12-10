#include "RenderDAGImNodeSaver.h"

#ifdef DEBUG_FEATURES_ENABLE

using namespace szg;

#include <unordered_set>

#include "Engine/Application/Logger.h"
#include "Engine/Assets/Json/JsonAsset.h"
#include "Engine/Loader/RenderPath/RenderNodeType.h"

void RenderDAGImNodeSaver::entry_point(const std::filesystem::path& filePath, const std::unordered_map<u64, EditorRenderDAG::DAGNodeType>& nodes) {
	std::unordered_map<u64, bool> seen;
	for (auto& [id_, _] : nodes) {
		seen[id_] = false;
	}

	std::unordered_map<u64, std::vector<u64>> outputLinks;
	for (auto& [id, node] : nodes) {
		for (auto& in : std::visit([](const std::shared_ptr<ImFlow::BaseNode>& n) { return n->getIns(); }, node)) {
			if (in->isConnected()) {
				std::shared_ptr<ImFlow::Link> link = in->getLink().lock();
				if (link) {
					IRenderDagImNode* outNode = static_cast<IRenderDagImNode*>(link->left()->getParent());
					u64 outNodeId = outNode->node_id();
					outputLinks[outNodeId].emplace_back(id);
				}
			}
		}
	}

	// WorldLayerRenderImNodeのLinkチェック
	for (auto& [id, node] : nodes) {
		if (std::holds_alternative<std::shared_ptr<WorldLayerRenderImNode>>(node)) {
			i32 numWorldLayerOutputs = 0;
			for (u64& outId : outputLinks[id]) {
				if (std::holds_alternative<std::shared_ptr<WorldLayerRenderImNode>>(nodes.at(outId))) {
					++numWorldLayerOutputs; // DAG内のWorldLayerRenderImNodeへの出力をカウント
				}
			}
			// WorldLayerRenderImNodeが存在する場合、他の接続ノードが存在してはいけない
			if (numWorldLayerOutputs >= 1 && outputLinks[id].size() >= 2) {
				szgWarning("Invalid link detected in RenderDAG in EditorRenderDAG::save()");
				return;
			}
		}
	}

	std::vector<EditorRenderDAG::DAGNodeType> sortedNodes;
	std::unordered_set<u64> visiting;

	// トポロジカルソート
	auto sort_dag = [&](u64 id, auto&& sort_dag_ref) {
		// ノード存在チェック
		if (!nodes.contains(id)) {
			szgWarning("Node id {} is missing in EditorRenderDAG::save()", id);
			return;
		}

		// サイクルチェック
		if (visiting.contains(id)) {
			szgWarning("Cycle detected in RenderDAG in EditorRenderDAG::save()");
			return;
		}

		// 訪問チェック
		if (seen[id]) {
			return;
		}
		seen[id] = true;
		auto& node = nodes.at(id);

		visiting.emplace(id);

		for (auto& in : std::visit([](const std::shared_ptr<ImFlow::BaseNode>& n) { return n->getIns(); }, node)) {
			// 接続チェック
			if (!in->isConnected()) {
				szgInformation("Input pin is not connected in EditorRenderDAG::save()");
				continue;
			}
			std::shared_ptr<ImFlow::Link> link = in->getLink().lock();
			// リンクチェック
			if (!link) {
				szgWarning("Failed to lock link in EditorRenderDAG::save()");
				continue;
			}
			// 接続先ノード取得
			IRenderDagImNode* outPin = static_cast<IRenderDagImNode*>(link->left()->getParent());
			// ノードID
			u64 nextNodeId = outPin->node_id();
			// 再帰実行
			sort_dag_ref(nextNodeId, sort_dag_ref);
		}

		visiting.erase(id);
		// 帰りがけ順で追加
		sortedNodes.emplace_back(node);
	};

	sort_dag(0, sort_dag);

	// サイクルチェック
	if (!visiting.empty()) {
		szgError("RenderDAG contains cycle(s), cannot save.");
		return;
	}

	// リザルトノードの削除
	sortedNodes.pop_back();

	// ノードIDからソート済みインデックスへの変換テーブルを作成
	std::unordered_map<u64, u64> sortedNodesInverse;
	for (u64 i = 0; const EditorRenderDAG::DAGNodeType& node : sortedNodes) {
		u64 nodeId = std::visit([](const std::shared_ptr<IRenderDagImNode>& n) { return n->node_id(); }, node);
		sortedNodesInverse[nodeId] = i;
		++i;
	}

	// 入力ピンの接続先ノードIDを収集
	std::unordered_map<u64, std::vector<std::pair<std::string, u64>>> outputTargetNodeIndices;
	for (auto& [id, node] : nodes) {
		for (auto& inPin : std::visit([](const std::shared_ptr<ImFlow::BaseNode>& n) { return n->getIns(); }, node)) {
			if (!inPin->isConnected()) {
				continue;
			}
			auto link = inPin->getLink().lock();
			if (!link) {
				continue;
			}
			u64 inNodeId = static_cast<IRenderDagImNode*>(link->right()->getParent())->node_id();
			u64 outNodeId = static_cast<IRenderDagImNode*>(link->left()->getParent())->node_id();
			outputTargetNodeIndices[inNodeId].emplace_back(inPin->getName(), sortedNodesInverse[outNodeId]);
		}
	}

	// ---------- 保存 ----------
	JsonAsset output{ filePath / "RenderPath.json" }; // 書き出し用ファイル
	// 既にある内容を削除
	output.get().clear();
	nlohmann::json& outputJson = output.get();
	nlohmann::json& nodesJson = outputJson["Nodes"];
	nodesJson = nlohmann::json::array();

	for (auto& node : sortedNodes) {
		// ノードのベース情報をJson化
		nlohmann::json nodeJson = std::visit(*this, node);
		// 接続状況の保存
		nlohmann::json& inLinksJson = nodeJson["Links"];
		inLinksJson = nlohmann::json::object();
		u64 nodeId = std::visit([](const std::shared_ptr<IRenderDagImNode>& node) { return node->node_id(); }, node);
		for (auto& [name, id] : outputTargetNodeIndices[nodeId]) {
			nlohmann::json& linksJson = inLinksJson[name];
			linksJson = id;
		}
		// JsonAssetに追加
		nodesJson.emplace_back(nodeJson);
	}
	// ResultNode
	{
		nlohmann::json resultJson = nlohmann::json::object();
		if (outputTargetNodeIndices[0].empty()) {
			resultJson["Connect"] = nullptr;
		}
		else {
			resultJson["Connect"] = outputTargetNodeIndices[0][0].second + 1;
		}
		const auto& node = std::get<3>(nodes.at(0));
		resultJson["Debug"]["Position"]["X"] = node->getPos().x;
		resultJson["Debug"]["Position"]["Y"] = node->getPos().y;
		// 追加
		outputJson["ResultNode"] = resultJson;
	}
	// ファイル書き出し
	output.save();
}

nlohmann::json RenderDAGImNodeSaver::operator()(const std::shared_ptr<WorldLayerRenderImNode>& node) {
	if (!node) {
		return {};
	}
	nlohmann::json json;
	json["Type"] = RenderNodeType::WorldLayer;
	const WorldLayerRenderImNode::Data& data = node->get_data();
	json["Data"].update(data.outputSize);
	json["Data"].update(data.worldIndex);
	json["Data"].update(data.layerIndex);
	json["Data"].update(data.cameraId);
	json["Data"].update(data.gBufferSize);
	json["Data"].update(data.gBufferViewport.left);
	json["Data"].update(data.gBufferViewport.top);
	json["Data"].update(data.gBufferViewport.width);
	json["Data"].update(data.gBufferViewport.height);
	json["Data"].update(data.gBufferViewport.minDepth);
	json["Data"].update(data.gBufferViewport.maxDepth);
	json["Data"].update(data.layerScissor.topLeft);
	json["Data"].update(data.layerScissor.bottomRight);

	json["Debug"]["Position"]["X"] = node->getPos().x;
	json["Debug"]["Position"]["Y"] = node->getPos().y;
	return json;
}

nlohmann::json RenderDAGImNodeSaver::operator()(const std::shared_ptr<PostEffectImNode>& node) {
	if (!node) {
		return {};
	}
	nlohmann::json json;
	json["Type"] = RenderNodeType::PostEffect;
	const PostEffectImNode::Data& data = node->get_data();
	json["Data"].update(data.outputSize);
	json["Data"].update(data.peType);
	if (data.isUseRuntime) {
		json["Data"].update(data.effectTagName);
	}
	else {
		json["Data"][data.effectTagName.label()] = nullptr;
	}

	json["Debug"]["Position"]["X"] = node->getPos().x;
	json["Debug"]["Position"]["Y"] = node->getPos().y;
	return json;
}

nlohmann::json RenderDAGImNodeSaver::operator()(const std::shared_ptr<StaticTextureImNode>& node) {
	if (!node) {
		return {};
	}
	nlohmann::json json;
	json["Type"] = RenderNodeType::StaticTexture;
	const StaticTextureImNode::Data& data = node->get_data();
	json["Data"].update(data.textureFilename);

	json["Debug"]["Position"]["X"] = node->getPos().x;
	json["Debug"]["Position"]["Y"] = node->getPos().y;
	return json;
}

nlohmann::json RenderDAGImNodeSaver::operator()(const std::shared_ptr<ResultImNode>&) {
	return {};
}

#endif // DEBUG_FEATURES_ENABLE
