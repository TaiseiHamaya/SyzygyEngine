#include "EditorRenderDAG.h"

#ifdef DEBUG_FEATURES_ENABLE

using namespace szg;

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <imgui.h>
#include <ImNodeFlow/include/ImNodeFlow.h>

#include "../Adapter/RenderDAGImNodeSaver.h"
#include "../Adapter/RenderDAGImNodeLoader.h"
#include "./RenderDagImNode/PostEffectImNode.h"
#include "./RenderDagImNode/ResultImNode.h"
#include "./RenderDagImNode/StaticTextureImNode.h"
#include "./RenderDagImNode/WorldLayerRenderImNode.h"
#include "./RenderDagImNode/IRenderDagImNode.h"
#include "Engine/Application/Logger.h"

EditorRenderDAG::EditorRenderDAG() {
	isActive = true;
}

void EditorRenderDAG::initialize() {
	imNodeFlow = std::make_unique<ImFlow::ImNodeFlow>();
	nodes.clear();
	imNodeFlow->rightClickPopUpContent([&](ImFlow::BaseNode* node) {
		if (node) { // ノードにホバー時
			if (ImGui::MenuItem("Delete")) {
				if (node->getName() == "ScreenOut") {
					szgWarning("Don't delete ScreenOutNode!");
				}
				else {
					node->destroy();
				}
			}
		}
		else {
			if (ImGui::MenuItem("WorldRender")) {
				auto imNode = imNodeFlow->placeNode<WorldLayerRenderImNode>();
				imNode->set_node_id(nodeCounter);
				nodes.emplace(nodeCounter, imNode);
				++nodeCounter;
			}
			if (ImGui::MenuItem("PostEffect")) {
				auto imNode = imNodeFlow->placeNode<PostEffectImNode>();
				imNode->set_node_id(nodeCounter);
				nodes.emplace(nodeCounter, imNode);
				++nodeCounter;
			}
			if (ImGui::MenuItem("StaticTexture")) {
				auto imNode = imNodeFlow->placeNode<StaticTextureImNode>();
				imNode->set_node_id(nodeCounter);
				nodes.emplace(nodeCounter, imNode);
				++nodeCounter;
			}
		}
	});
	imNodeFlow->droppedLinkPopUpContent([&](ImFlow::Pin* pin) {
		bool isInPin = pin->getType() == ImFlow::PinType::PinType_Input;
		if (ImGui::MenuItem("WorldRender")) {
			auto imNode = imNodeFlow->placeNode<WorldLayerRenderImNode>();
			imNode->set_node_id(nodeCounter);
			if (isInPin) {
				auto& outPin = imNode->getOuts().front();
				outPin->createLink(pin);
			}
			else {
				auto& inPin = imNode->getIns().front();
				pin->createLink(inPin.get());
			}
			nodes.emplace(nodeCounter, imNode);
			++nodeCounter;
		}
		if (ImGui::MenuItem("PostEffect")) {
			auto imNode = imNodeFlow->placeNode<PostEffectImNode>();
			imNode->set_node_id(nodeCounter);
			if (isInPin) {
				auto& outPin = imNode->getOuts().front();
				outPin->createLink(pin);
			}
			else {
				auto& inPin = imNode->getIns().front();
				pin->createLink(inPin.get());
			}
			nodes.emplace(nodeCounter, imNode);
			++nodeCounter;
		}
		if (isInPin) {
			if (ImGui::MenuItem("StaticTexture")) {
				auto imNode = imNodeFlow->placeNode<StaticTextureImNode>();
				imNode->set_node_id(nodeCounter);
				auto& outPin = imNode->getOuts().front();
				outPin->createLink(pin);
				nodes.emplace(nodeCounter, imNode);
				++nodeCounter;
			}
		}
	});
	generate_result_node();
	nodeCounter = 1;
}

void EditorRenderDAG::load(const std::string& sceneName) {
	initialize();
	nodeCounter = RenderDAGImNodeLoader{}.entry_point(sceneName, imNodeFlow, nodes);
}

void EditorRenderDAG::finalize() {
	imNodeFlow.reset();
}

void EditorRenderDAG::draw() {
	if (!isActive) {
		return;
	}

	int flags = 0;
	ImGui::Begin("Render DAG Editor", &isActive, flags);

	update_focus();

	if (!nodes.contains(0)) {
		szgWarning("Don't delete ScreenOutNode!");
		generate_result_node();
	}

	std::erase_if(nodes, [](const std::pair<u64, DAGNodeType>& pair) {
		return std::visit([](const std::shared_ptr<ImFlow::BaseNode>& node) { return node->toDestroy(); }, pair.second);
	});

	imNodeFlow->setSize(ImGui::GetContentRegionAvail());
	ImGui::PushStyleColor(ImGuiCol_BorderSelected, ImVec4{});
	imNodeFlow->update();
	ImGui::PopStyleColor();

	ImGui::End();
}

void EditorRenderDAG::save(const std::filesystem::path& filePath) {
	RenderDAGImNodeSaver{}.entry_point(filePath, nodes);
}

const std::unordered_map<u64, EditorRenderDAG::DAGNodeType>& EditorRenderDAG::nodes_imm() const {
	return nodes;
}

void EditorRenderDAG::generate_result_node() {
	auto resultNode = imNodeFlow->addNode<ResultImNode>({ 600, 350 });
	nodes.emplace(0, resultNode);
	resultNode->set_node_id(0);
}

#endif // DEBUG_FEATURES_ENABLE
