#include "WorldRenderCollection.h"

using namespace szg;

#include "Engine/Application/Logger.h"
#include "Engine/Module/Manager/World/InstanceBucket.h"

void WorldRenderCollection::initialize() {
}

void WorldRenderCollection::setup(u8 numLayer_) {
	if (numLayer_ >= 32) { // レイヤー数は最大32
		szgWarning("Layer value \'{}\' exceeds maximum of 31. Clamped to 31.", numLayer_);
		numLayer_ = 31;
	}
	numLayer = numLayer_;

	// DrawManager
	staticMeshDrawManager.initialize(numLayer);
	skinningMeshDrawManager.initialize(numLayer);
	rect3dDrawManager.initialize(numLayer);
	stringRectDrawManager.initialize(numLayer);

	// ライティング関連
	directionalLightingExecutors.resize(numLayer);
	pointLightingExecutors.resize(numLayer);
	for (u8 i = 0; i < numLayer; ++i) {
		// 設置可能Instance
		directionalLightingExecutors[i].reinitialize(4); // TODO: 可変長にする
		pointLightingExecutors[i].reinitialize(1024);
	}
}

void WorldRenderCollection::remove_marked_destroy() {
	// 各種削除予定のインスタンスを描画対象から外す
	staticMeshDrawManager.remove_marked_destroy();
	skinningMeshDrawManager.remove_marked_destroy();
	rect3dDrawManager.remove_marked_destroy();
	stringRectDrawManager.remove_marked_destroy();
	std::erase_if(directionalLights, [](const Reference<DirectionalLightInstance>& instance) {
		return instance->is_marked_destroy();
	});
	std::erase_if(pointLights, [](const Reference<PointLightInstance>& instance) {
		return instance->is_marked_destroy();
	});
}

void WorldRenderCollection::collect_instantiated(Reference<InstanceBucket> instanceBucket) {
	// StaticMesh
	for (auto& instance : instanceBucket->staticMesh) {
		staticMeshDrawManager.register_instance(instance);
	}
	// SkinningMesh
	for (auto& instance : instanceBucket->skinMesh) {
		skinningMeshDrawManager.register_instance(instance);
	}
	// Rect3d
	for (auto& instance : instanceBucket->rect) {
		rect3dDrawManager.register_instance(instance);
	}
	// StringRect
	for (auto& instance : instanceBucket->stringRect) {
		stringRectDrawManager.register_instance(instance);
	}

	// Directional Light
	for (auto& instance : instanceBucket->directionalLightInstance) {
		directionalLights.emplace_back(instance);
	}
	// Point Light
	for (auto& instance : instanceBucket->pointLightInstance) {
		pointLights.emplace_back(instance);
	}
	// Camera
	for (auto& instance : instanceBucket->camera) {
		cameras.emplace_back(instance);
	}
}

void WorldRenderCollection::transfer() {
	// StaticMesh
	staticMeshDrawManager.transfer();
	// SkinningMesh
	skinningMeshDrawManager.transfer();
	// Rect3d
	rect3dDrawManager.transfer();
	// StringRect
	stringRectDrawManager.transfer();

	for (auto& lightExecutor : directionalLightingExecutors) {
		lightExecutor.begin();
	}
	for(auto& lightExecutor : pointLightingExecutors) {
		lightExecutor.begin();
	}
	for (auto& instance : directionalLights) {
		u32 influenceLayer = instance->influence_layer();
		for(u8 i = 0; i < numLayer; ++i) {
			if (influenceLayer & (1u << i)) {
				directionalLightingExecutors[i].write_to_buffer(instance);
			}
		}
	}
	for (auto& instance : pointLights) {
		u32 influenceLayer = instance->influence_layer();
		for (u8 i = 0; i < numLayer; ++i) {
			if (influenceLayer & (1u << i)) {
				pointLightingExecutors[i].write_to_buffer(instance);
			}
		}
	}
}

Reference<CameraInstance> WorldRenderCollection::camera_at(u32 index) const {
	if (index >= cameras.size()) {
		return nullptr;
	}
	return cameras[index];
}
