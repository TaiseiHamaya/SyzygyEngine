#include "Rect3dDrawManager.h"

using namespace szg;

void Rect3dDrawManager::make_instancing(u32 layer, const BlendMode& blendMode, u32 maxInstance) {
	if (layer >= maxLayer) {
		return;
	}
	auto key = std::make_pair(layer, blendMode);
	if (executors.contains(key)) {
		return;
	}

	// 追加
	Rect3dDrawExecutor& executor = executors[key];
	executor.reinitialize(
		blendMode, maxInstance
	);

	layerExecutors[layer].emplace_back(executor);
}
