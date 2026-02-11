#include "WorldLayerRenderImNode.h"

#ifdef DEBUG_FEATURES_ENABLE

using namespace szg;

WorldLayerRenderImNode::WorldLayerRenderImNode() {
	setTitle("WorldLayerRender");
	setStyle(ImFlow::NodeStyle::red());

	addIN<u64>("Base", 0, ImFlow::ConnectionFilter::SameType());
	addOUT<u64>("Output", nullptr)->behaviour([&]() {
		return nodeId;
	});
}

void WorldLayerRenderImNode::draw() {
	ImGui::PushItemWidth(120.0f);
	ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_SpanLabelWidth;

	data.outputSize.show_gui();

	ImGui::Text("World/Layer");

	data.worldIndex.show_gui();
	data.layerIndex.show_gui();
	data.cameraId.show_gui();

	ImGui::Text("GBuffer");

	data.gBufferSize.show_gui();

	ImGui::Text("Rendering");

	data.isClearDepthStencil.show_gui();

	if (ImGui::TreeNodeEx("Viewport", treeNodeFlags)) {
		data.gBufferViewport.left.show_gui();
		data.gBufferViewport.top.show_gui();
		data.gBufferViewport.width.show_gui();
		data.gBufferViewport.height.show_gui();
		data.gBufferViewport.minDepth.show_gui();
		data.gBufferViewport.maxDepth.show_gui();

		ImGui::TreePop();
	}
	
	if (ImGui::TreeNodeEx("Scissor", treeNodeFlags)) {
		data.layerScissor.topLeft.show_gui();
		data.layerScissor.bottomRight.show_gui();
		ImGui::TreePop();
	}

	ImGui::PopItemWidth();
}

void WorldLayerRenderImNode::set_data(const Data& data_) {
	data = data_;
}

const WorldLayerRenderImNode::Data& WorldLayerRenderImNode::get_data() const {
	return data;
}

#endif // DEBUG_FEATURES_ENABLE
