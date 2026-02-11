#include "StaticMeshDrawManager.h"

using namespace szg;

#include <execution>

#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"

void StaticMeshDrawManager::make_instancing(u32 layer, const std::string& meshName_, u32 maxInstance) {
	if (layer >= maxLayer) {
		return;
	}
	if (!PolygonMeshLibrary::IsRegistered(meshName_)) {
		return;
	}

	std::pair<u32, std::string> key = std::make_pair(layer, meshName_);
	auto [emplaced, result] = executors.try_emplace(
		key,
		PolygonMeshLibrary::GetPolygonMesh(meshName_), maxInstance
	);

	if (result) {
		layerExecutors[layer].emplace_back(emplaced->second);
	}
}
