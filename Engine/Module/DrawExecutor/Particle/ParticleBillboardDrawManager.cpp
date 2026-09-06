#include "ParticleBillboardDrawManager.h"

using namespace szg;

void ParticleBillboardDrawManager::make_instancing(u32 layer, const BlendMode& blendMode, u32 maxInstance) {
	if (layer >= maxLayer) {
		return;
	}
	auto key = std::make_pair(layer, blendMode);
	if (executors.contains(key)) {
		return;
	}

	ParticleBillboardDrawExecutor& executor = executors[key];
	executor.reinitialize(blendMode, maxInstance);

	layerExecutors[layer].emplace_back(executor);
}

void ParticleBillboardDrawManager::ensure_instancing(u32 layer, const BlendMode& blendMode, u32 maxInstance) {
	make_instancing(layer, blendMode, maxInstance);
	auto key = std::make_pair(layer, blendMode);
	if (!executors.contains(key)) {
		return;
	}
	if (executors.at(key).max_instance() < maxInstance) {
		executors.at(key).reinitialize(blendMode, maxInstance);
	}
}

void ParticleBillboardDrawManager::transfer_item(Reference<const ParticleDrawItem> item) {
	if (!item) {
		return;
	}
	auto key = std::make_pair(item->layer(), item->key_id());
	if (!executors.contains(key)) {
		return;
	}
	executors.at(key).write_to_buffer(item);
}
