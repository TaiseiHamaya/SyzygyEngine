#include "SkeletonAsset.h"

using namespace szg;

SkeletonAsset::SkeletonAsset(Skeleton& skeleton_, std::unordered_map<std::string, VertexBuffer<VertexInfluenceData>>& influenceBuffers_) :
	skeletonData(std::move(skeleton_)), 
	influenceBuffers(std::move(influenceBuffers_)) {
}

const Skeleton& SkeletonAsset::skeleton() const {
	return skeletonData;
}

const VertexBuffer<VertexInfluenceData>* SkeletonAsset::weight_influence(const std::string& bindMeshName) const {
	if (influenceBuffers.contains(bindMeshName)) {
		return &influenceBuffers.at(bindMeshName);
	}
	return nullptr;
}

const std::vector<u32>* SkeletonAsset::use_joint_indexes(const std::string& bindMeshName) const {
	if (skeletonData.useJointIndexesByMeshName.contains(bindMeshName)) {
		return &skeletonData.useJointIndexesByMeshName.at(bindMeshName);
	}
	return nullptr;
}

u32 SkeletonAsset::joint_size() const {
	return static_cast<u32>(skeletonData.joints.size());
}
