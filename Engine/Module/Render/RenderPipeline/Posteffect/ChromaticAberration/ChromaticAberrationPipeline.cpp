#include "ChromaticAberrationPipeline.h"

#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"

#ifdef DEBUG_FEATURES_ENABLE
#include <imgui.h>
#endif // _DEBUG

using namespace szg;

ChromaticAberrationPipeline::ChromaticAberrationPipeline() = default;

ChromaticAberrationPipeline::~ChromaticAberrationPipeline() noexcept = default;

void ChromaticAberrationPipeline::initialize() {
	create_pipeline_state();
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void ChromaticAberrationPipeline::execute_effect_command() {
	baseTexture->start_read();

	auto&& command = DxCommand::GetCommandList();
	command->SetGraphicsRootConstantBufferView(0, data.get_resource()->GetGPUVirtualAddress());
	baseTexture->get_as_srv()->use(1);
	command->DrawInstanced(3, 1, 0, 0);
}

void ChromaticAberrationPipeline::set_shader_texture(Reference<RenderTexture> baseTexture_) {
	baseTexture = baseTexture_;
}

Reference<ChromaticAberrationPipeline::Data> ChromaticAberrationPipeline::data_mut() noexcept {
	return data.data_mut();
}

void ChromaticAberrationPipeline::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 0, 1);
	rootSignatureBuilder.sampler(
		D3D12_SHADER_VISIBILITY_PIXEL,
		0
	);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate();
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "FullscreenShader.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "ChromaticAberration.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());

}

#ifdef DEBUG_FEATURES_ENABLE
void ChromaticAberrationPipeline::debug_gui() {
	ImGui::DragFloat("AberrationLevelX", &data.data_mut()->aberrationLevel.x, 0.1f / ProjectSettings::ClientWidth(), -0.5f, 0.5f, "%.4f");
	ImGui::DragFloat("AberrationLevelY", &data.data_mut()->aberrationLevel.y, 0.1f / ProjectSettings::ClientHeight(), -0.5f, 0.5f, "%.4f");
}
#endif // _DEBUG
