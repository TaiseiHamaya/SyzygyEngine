#include "GaussianBlurPipeline.h"

#include <Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h>
#include <Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h>
#include <Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h>

using namespace szg;

void GaussianBlurPipeline::initialize() {
	create_pipeline_state();
	pipelineState->set_name("GaussianBlurPipeline");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	data.data_mut()->dispersion = 0.4f;
	data.data_mut()->length = 10.0f;
	data.data_mut()->sampleCount = 4;
}

void GaussianBlurPipeline::execute_effect_command() {
	baseTexture->start_read();
	auto&& command = DxCommand::GetCommandList();
	command->SetGraphicsRootConstantBufferView(0, data.get_resource()->GetGPUVirtualAddress());
	baseTexture->get_as_srv()->use(1);
	command->DrawInstanced(3, 1, 0, 0);
}

void GaussianBlurPipeline::set_base_texture(Reference<RenderTexture> baseTexture_) {
	baseTexture = baseTexture_;
}

void GaussianBlurPipeline::set_parameters(r32 dispersion, r32 length, u32 sampleCount) {
	*data.data_mut() = {
		.dispersion = dispersion,
		.length = length,
		.sampleCount = sampleCount
	};
}

Reference<GaussianBlurPipeline::Data> GaussianBlurPipeline::data_mut() noexcept {
	return data.data_mut();
}

void GaussianBlurPipeline::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignatureBuilder.sampler(
		D3D12_SHADER_VISIBILITY_PIXEL,
		0, 0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate();
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "FullscreenShader.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "GaussianBlur.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());
}

#ifdef DEBUG_FEATURES_ENABLE
#include <imgui.h>
void GaussianBlurPipeline::debug_gui() {
	ImGui::DragFloat("Weight", &data.data_mut()->dispersion, 0.001f, 0.0f, 1.0f, "%.4f");
	ImGui::DragFloat("Length", &data.data_mut()->length, 0.01f);
	constexpr u32 min = 1;
	constexpr u32 max = 16;
	ImGui::DragScalar("SampleCount", ImGuiDataType_U32, reinterpret_cast<int*>(&data.data_mut()->sampleCount), 0.02f, &min, &max);
}
#endif // DEBUG_FEATURES_ENABLE
