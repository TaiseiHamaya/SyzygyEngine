#include "SkinningMeshDrawManager.h"

using namespace szg;

#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"
#include "Engine/Assets/Animation/Skeleton/SkeletonLibrary.h"

void SkinningMeshDrawManager::make_instancing(u32 layer, const std::string& meshName, u32 maxInstance) {
	if (layer >= maxLayer) {
		return;
	}
	if (!PolygonMeshLibrary::IsRegistered(meshName) ||
		!SkeletonLibrary::IsRegistered(meshName)) {
		return;
	}

	std::pair<u32, std::string> key = std::make_pair(layer, meshName);
	auto [emplaced, result] = executors.try_emplace(
		key,
		PolygonMeshLibrary::GetPolygonMesh(meshName),
		SkeletonLibrary::GetSkeleton(meshName),
		maxInstance
	);

	if (result) {
		layerExecutors[layer].emplace_back(emplaced->second);
	}
}
