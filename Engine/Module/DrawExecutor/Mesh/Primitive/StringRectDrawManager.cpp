#include "StringRectDrawManager.h"

using namespace szg;

void StringRectDrawManager::make_instancing(u32 layer, const BlendMode& blendMode, u32 maxRenderingChar) {
	make_instancing(layer, blendMode, maxRenderingChar, maxRenderingChar);
}

void StringRectDrawManager::make_instancing(u32 layer, const BlendMode& blendMode, u32 maxRenderingChar, u32 maxInstance) {
	if (layer >= maxLayer) {
		// TODO: エラーログを出す
		return;
	}
	auto key = std::make_pair(layer, blendMode);
	if (executors.contains(key)) {
		return;
	}

	// 追加
	StringRectDrawExecutor& executor = executors[key];
	executor.reinitialize(
		blendMode, maxRenderingChar, maxInstance
	);
	layerExecutors[layer].emplace_back(executor);
}
