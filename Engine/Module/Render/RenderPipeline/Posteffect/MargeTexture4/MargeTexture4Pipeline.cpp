#include "MargeTexture4Pipeline.h"

#include <Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h>
#include <Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h>
#include <Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h>

using namespace szg;

void MargeTexture4Pipeline::initialize() {
	create_pipeline_state();
	pipelineState->set_name("MargeTexture4Pipeline");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void MargeTexture4Pipeline::execute_effect_command() {
	for (auto& texture : baseTextures) {
		texture->start_read();
	}

	auto&& command = DxCommand::GetCommandList();
	for (int i = 0; i < 4; ++i) { // 0-3でSRVを設定
		baseTextures[i]->get_as_srv()->use(i);
	}
	command->DrawInstanced(3, 1, 0, 0);
}

void MargeTexture4Pipeline::set_texture_resources(std::array<Reference<RenderTexture>, 4> baseTextures_) {
	baseTextures = baseTextures_;
}

void MargeTexture4Pipeline::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 1);
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 2);
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 3);
	rootSignatureBuilder.sampler(
		D3D12_SHADER_VISIBILITY_PIXEL,
		0, 0,
		D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate();
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "FullscreenShader.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "MargeTexture4.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());

}
