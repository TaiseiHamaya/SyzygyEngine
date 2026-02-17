#include "LuminanceExtractionPipeline.h"

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"

using namespace szg;

void LuminanceExtractionPipeline::initialize() {
	create_pipeline_state();
	pipelineState->set_name("LuminanceExtractionPipeline");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	data.data_mut()->intensity = 0.55f;
}

void LuminanceExtractionPipeline::execute_effect_command() {
	baseTexture->start_read();

	auto&& command = DxCommand::GetCommandList();
	command->SetGraphicsRootConstantBufferView(0, data.get_resource()->GetGPUVirtualAddress());
	baseTexture->get_as_srv()->use(1);
	command->DrawInstanced(3, 1, 0, 0);
}

void LuminanceExtractionPipeline::set_texture_resource(Reference<RenderTexture> baseTexture_) {
	baseTexture = baseTexture_;
}

Reference<LuminanceExtractionPipeline::Data> LuminanceExtractionPipeline::data_mut() noexcept {
	return data.data_mut();
}

void LuminanceExtractionPipeline::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate();
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "FullscreenShader.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "LuminanceExtraction.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());
}
