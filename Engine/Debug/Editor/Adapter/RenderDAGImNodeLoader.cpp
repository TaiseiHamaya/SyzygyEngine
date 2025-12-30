#include "RenderDAGImNodeLoader.h"

#ifdef DEBUG_FEATURES_ENABLE

using namespace szg;

#include <vector>

#include "../Window/RenderDagImNode/PostEffectImNode.h"
#include "../Window/RenderDagImNode/ResultImNode.h"
#include "../Window/RenderDagImNode/StaticTextureImNode.h"
#include "../Window/RenderDagImNode/WorldLayerRenderImNode.h"
#include "Engine/Assets/Json/JsonAsset.h"
#include "Engine/Loader/RenderPath/RenderNodeType.h"

u64 RenderDAGImNodeLoader::entry_point(const std::string& sceneName, Reference<ImFlow::ImNodeFlow> imNodeFlow_, std::unordered_map<u64, EditorRenderDAG::DAGNodeType>& nodes) {
	imNodeFlow = imNodeFlow_;

	// ファイル読み込み
	JsonAsset input{ "./Game/Core/Scene/" + sceneName + "/RenderPath.json" };
	counter = imNodeFlow->getNodesCount();

	if (input.get().is_null()) {
		szgWarning("RenderPath.jsonの読み込みに失敗しました: {}", sceneName);
		return counter;
	}

	for (const auto& node : input.get().value("Nodes", nlohmann::json::array())) {
		ImFlow::BaseNode* imNode{ nullptr };
		const RenderNodeType type = node.value("Type", RenderNodeType::Unknown);
		switch (type) {
		case RenderNodeType::WorldLayer:
		{
			auto temp = load_as_world_render(node["Data"]);
			nodes.emplace(counter, temp);
			imNode = temp.get();
			break;
		}
		case RenderNodeType::PostEffect:
		{
			auto temp = load_as_post_effect(node["Data"]);
			nodes.emplace(counter, temp);
			imNode = temp.get();
			break;
		}
		case RenderNodeType::StaticTexture:
		{
			auto temp = load_as_static_texture(node["Data"]);
			nodes.emplace(counter, temp);
			imNode = temp.get();
			break;
		}
		default:
			break;
		}

		imNode->setPos({
			node.value("Debug", nlohmann::json::object()).value("Position", nlohmann::json::object()).value("X", 0.0f),
			node.value("Debug", nlohmann::json::object()).value("Position", nlohmann::json::object()).value("Y", 0.0f)
		});

		// 接続状況の解決
		if (imNode) {
			std::vector<std::shared_ptr<ImFlow::Pin>> inPins = imNode->getIns();
			for (auto& [name, connect] : node["Links"].items()) {
				ImFlow::Pin* inPin = imNode->inPin(name);
				if (!inPin) {
					szgWarning("ノード内にピンが見つかりませんでした: {}", name);
					continue;
				}
				u64 nodeIndex = connect.get<u64>();
				ImFlow::Pin* outPin = std::visit([](std::shared_ptr<ImFlow::BaseNode> node) { return node->outPin("Output"); }, nodes[nodeIndex + 1]);
				if (!outPin) {
					szgWarning("接続先ノードの出力ピンが見つかりませんでした: {}", nodeIndex);
					continue;
				}

				outPin->createLink(inPin);
			}
		}

		++counter;
	}

	// ResultNodeとの接続
	const nlohmann::json& resultJson = input.cget()["ResultNode"];
	std::shared_ptr<ResultImNode> resultNode = std::get<3>(nodes[0]);
	if (!resultJson["Connect"].is_null()) {
		EditorRenderDAG::DAGNodeType connectNode = nodes[resultJson["Connect"].get<u64>()];
		ImFlow::Pin* outPin = std::visit([](std::shared_ptr<ImFlow::BaseNode> node) { return node->outPin("Output"); }, connectNode);
		ImFlow::Pin* inPin = resultNode->inPin("DisplayOut");
		if (outPin && inPin) {
			outPin->createLink(inPin);
		}
	}
	resultNode->setPos({
		resultJson.value("Debug", nlohmann::json::object()).value("Position", nlohmann::json::object()).value("X", 0.0f),
		resultJson.value("Debug", nlohmann::json::object()).value("Position", nlohmann::json::object()).value("Y", 0.0f)
	});

	return counter;
}

std::shared_ptr<WorldLayerRenderImNode> RenderDAGImNodeLoader::load_as_world_render(const nlohmann::json& json) {
	auto result = imNodeFlow->addNode<WorldLayerRenderImNode>({});
	WorldLayerRenderImNode::Data data;
	json.get_to(data.outputSize);
	json.get_to(data.worldIndex);
	json.get_to(data.layerIndex);
	json.get_to(data.cameraId);
	json.get_to(data.isClearDepthStencil);

	json.get_to(data.gBufferSize);
	json.get_to(data.gBufferViewport.left);
	json.get_to(data.gBufferViewport.top);
	json.get_to(data.gBufferViewport.width);
	json.get_to(data.gBufferViewport.height);
	json.get_to(data.gBufferViewport.minDepth);
	json.get_to(data.gBufferViewport.maxDepth);
	
	json.get_to(data.layerScissor.topLeft);
	json.get_to(data.layerScissor.bottomRight);
	
	result->set_data(data);
	result->set_node_id(counter);
	return result;
}

std::shared_ptr<PostEffectImNode> RenderDAGImNodeLoader::load_as_post_effect(const nlohmann::json& json) {
	auto result = imNodeFlow->addNode<PostEffectImNode>({});
	PostEffectImNode::Data data;
	json.get_to(data.outputSize);
	json.get_to(data.peType);
	if (json.value("EffectTag", nlohmann::json{ nullptr }).is_null()) {
		data.isUseRuntime = false;
	}
	else {
		json.get_to(data.effectTagName);
	}
	result->set_data(data);
	result->set_node_id(counter);
	result->update_extra_input_pins();
	return result;
}

std::shared_ptr<StaticTextureImNode> RenderDAGImNodeLoader::load_as_static_texture(const nlohmann::json& json) {
	auto result = imNodeFlow->addNode<StaticTextureImNode>({});
	StaticTextureImNode::Data data;
	json.get_to(data.textureFilename);
	result->set_data(data);
	result->set_node_id(counter);
	return result;
}

#endif // DEBUG_FEATURES_ENABLE
