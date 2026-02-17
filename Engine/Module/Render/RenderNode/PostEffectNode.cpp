#include "PostEffectNode.h"

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"

using namespace szg;

void PostEffectNode::stack_command() {
	data.postEffectPSO->preprocess();

	auto&& commandList = DxCommand::GetCommandList();
	// ViewPortの設定
	commandList->RSSetViewports(1, &data.viewport);
	// シザー矩形の設定
	commandList->RSSetScissorRects(1, &data.rect);

	data.outputRenderTargetGroup->begin_write(true);

	data.postEffectPSO->stack_pso_command();

	data.postEffectPSO->execute_effect_command();
}
