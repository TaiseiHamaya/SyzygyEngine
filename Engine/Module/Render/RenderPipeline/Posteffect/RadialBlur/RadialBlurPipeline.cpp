#include "RadialBlurPipeline.h"

using namespace szg;

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"
#include "Engine/Module/Manager/RuntimeStorage/RuntimeStorage.h"

#ifdef DEBUG_FEATURES_ENABLE
#include <imgui.h>
#endif // _DEBUG

RadialBlurPipeline::RadialBlurPipeline() = default;

RadialBlurPipeline::~RadialBlurPipeline() noexcept = default;

void RadialBlurPipeline::initialize() {
	create_pipeline_state();
	pipelineState->set_name("RadialBlurPipeline");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	*data.data_mut() = Data{
		CVector2::HALF,
		0.4f,
		0.1f,
		1
	};
}

void RadialBlurPipeline::execute_effect_command() {
	baseTexture->start_read();

	auto&& command = DxCommand::GetCommandList();
	command->SetGraphicsRootConstantBufferView(0, data.get_resource()->GetGPUVirtualAddress());
	baseTexture->get_as_srv()->use(1);
	command->DrawInstanced(3, 1, 0, 0);
}

void RadialBlurPipeline::set_shader_texture(Reference<RenderTexture> baseTexture_) {
	baseTexture = baseTexture_;
}

Reference<RadialBlurPipeline::Data> RadialBlurPipeline::data_mut() noexcept {
	return data.data_mut();
}

void RadialBlurPipeline::create_pipeline_state() {
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
	psoBuilder->shaders(ShaderType::Pixel, "RadialBlur.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());

}

#ifdef DEBUG_FEATURES_ENABLE
void RadialBlurPipeline::debug_gui() {
	ImGui::DragFloat2("Center", &data.data_mut()->center.x, 0.01f, 0.0f, 1.0f, "%.4f");
	ImGui::DragFloat("Weight", &data.data_mut()->weight, 0.001f, 0.0f, 1.0f, "%.4f");
	ImGui::DragFloat("Length", &data.data_mut()->length, 0.001f, 0.0f, 1.0f, "%.4f");
	u32 min = 1, max = 16;
	ImGui::DragScalar("SampleCount", ImGuiDataType_U32, &data.data_mut()->sampleCount, 0.02f, &min, &max);
}
#endif // _DEBUG
