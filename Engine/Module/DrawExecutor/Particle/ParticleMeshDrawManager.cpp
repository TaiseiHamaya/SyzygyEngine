#include "ParticleMeshDrawManager.h"

#include <format>

#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"

using namespace szg;

void ParticleMeshDrawManager::make_instancing(u32 layer, const std::string& meshName, u32 maxInstance) {
	if (layer >= maxLayer) {
		return;
	}
	auto key = std::make_pair(layer, meshName);
	if (executors.contains(key)) {
		return;
	}

	std::string resolved = PolygonMeshLibrary::IsRegistered(meshName) ? meshName : "ErrorObject.obj";
	std::shared_ptr<const PolygonMesh> asset = PolygonMeshLibrary::GetPolygonMesh(resolved);
	ParticleMeshDrawExecutor& executor = executors[key];
	executor.reinitialize(asset, BlendMode::Alpha, maxInstance);

	layerExecutors[layer].emplace_back(executor);
}

void ParticleMeshDrawManager::ensure_instancing(u32 layer, const std::string& meshName_, BlendMode blendMode, u32 maxInstance, std::shared_ptr<const PolygonMesh> asset) {
	if (layer >= maxLayer || !asset) {
		return;
	}
	std::string key = MakeKey(meshName_, blendMode);
	auto mapKey = std::make_pair(layer, key);
	if (!executors.contains(mapKey)) {
		ParticleMeshDrawExecutor& executor = executors[mapKey];
		executor.reinitialize(asset, blendMode, maxInstance);
		layerExecutors[layer].emplace_back(executor);
	}
	else if (executors.at(mapKey).max_instance() < maxInstance) {
		executors.at(mapKey).reinitialize(asset, blendMode, maxInstance);
	}
}

void ParticleMeshDrawManager::transfer_item(Reference<const ParticleDrawItem> item) {
	if (!item) {
		return;
	}
	auto key = std::make_pair(item->layer(), MakeKey(item->mesh_key(), item->key_id()));
	if (!executors.contains(key)) {
		return;
	}
	executors.at(key).write_to_buffer(item);
}

std::string ParticleMeshDrawManager::MakeKey(const std::string& meshName_, BlendMode blendMode) {
	return std::format("{}\n{}", meshName_, static_cast<u32>(blendMode));
}
