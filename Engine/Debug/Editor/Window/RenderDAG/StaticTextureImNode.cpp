#include "StaticTextureImNode.h"

#ifdef DEBUG_FEATURES_ENABLE

using namespace szg;

StaticTextureImNode::StaticTextureImNode() {
	setTitle("StaticTexture");
	setStyle(ImFlow::NodeStyle::green());
	addOUT<u64>("Output", nullptr)->behaviour([&]() {
		return nodeId;
	});
}

void StaticTextureImNode::draw() {
	ImGui::PushItemWidth(120.f);
	data.textureFilename.show_gui();
	ImGui::PopItemWidth();
}

const StaticTextureImNode::Data& StaticTextureImNode::get_data() const {
	return data;
}

void StaticTextureImNode::set_data(const Data& data_) {
	data = data_;
}

#endif // DEBUG_FEATURES_ENABLE
