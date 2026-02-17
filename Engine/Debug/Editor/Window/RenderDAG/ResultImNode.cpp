#include "ResultImNode.h"

#ifdef DEBUG_FEATURES_ENABLE

using namespace szg;

ResultImNode::ResultImNode() {
	setTitle("ScreenOut");
	setStyle(ImFlow::NodeStyle::brown());
	ImFlow::BaseNode::addIN<u64>("DisplayOut", 0, ImFlow::ConnectionFilter::SameType());
}

void ResultImNode::draw() {
}

#endif DEBUG_FEATURES_ENABLE
