#include "GrayscalePipeline.h"

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"
#include "Engine/Runtime/RuntimeStorage/RuntimeStorage.h"

#ifdef DEBUG_FEATURES_ENABLE
#include <imgui.h>
#endif // _DEBUG

using namespace szg;

GrayscalePipeline::GrayscalePipeline() = default;

GrayscalePipeline::~GrayscalePipeline() = default;

void GrayscalePipeline::initialize() {
	create_pipeline_state();
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pipelineState->set_name("GrayscalePipeline");
}

void GrayscalePipeline::execute_effect_command() {
	baseTexture->start_read();

	auto&& command = DxCommand::GetCommandList();
	baseTexture->get_as_srv()->use(1);
	command->SetGraphicsRootConstantBufferView(0, isGray.get_resource()->GetGPUVirtualAddress());
	command->DrawInstanced(3, 1, 0, 0);
}

void GrayscalePipeline::set_shader_texture(Reference<RenderTexture> baseTexture_) {
	baseTexture = baseTexture_;
}

Reference<GrayscalePipeline::Data> GrayscalePipeline::data_mut() noexcept {
	return isGray.data_mut();
}

void GrayscalePipeline::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignatureBuilder.sampler(
		D3D12_SHADER_VISIBILITY_PIXEL,
		0
	);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate();
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "FullscreenShader.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "Grayscale.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());
}

#ifdef DEBUG_FEATURES_ENABLE
void GrayscalePipeline::debug_gui() {
	ImGui::Checkbox("IsGray", reinterpret_cast<bool*>(isGray.data_mut().ptr()));
}
#endif // _DEBUG
