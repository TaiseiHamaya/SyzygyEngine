#include "NodeAnimationAsset.h"

using namespace szg;

NodeAnimationAsset::NodeAnimationAsset(r32 _duration, std::unordered_map<std::string, NodeAnimation>& _nodeAnimations) :
	duration_(_duration),
	nodeAnimations(std::move(_nodeAnimations)) {
}

const NodeAnimationAsset::NodeAnimation& NodeAnimationAsset::node(const std::string& nodeName) const {
	return nodeAnimations.at(nodeName);
}

bool NodeAnimationAsset::contains(const std::string& nodeName) const {
	return nodeAnimations.contains(nodeName);
}
