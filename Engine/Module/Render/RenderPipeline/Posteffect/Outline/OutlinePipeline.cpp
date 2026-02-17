#include "OutlinePipeline.h"

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/DepthStencilTexture.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/RenderTexture.h"

using namespace szg;

OutlinePipeline::OutlinePipeline() = default;

OutlinePipeline::~OutlinePipeline() = default;

void OutlinePipeline::initialize() {
	create_pipeline_state();
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pipelineState->set_name("OutlinePipeline");
}

void OutlinePipeline::preprocess() {
}

void OutlinePipeline::execute_effect_command() {
	// リード用に使用
	baseTexture->start_read();
	depthTexture->start_read();
	// コマンド設定
	baseTexture->get_as_srv()->use(0);
	depthTexture->get_as_srv()->use(1);
	auto&& command = DxCommand::GetCommandList();
	command->DrawInstanced(3, 1, 0, 0);
}

void OutlinePipeline::set_shader_texture(Reference<RenderTexture> baseTexture_, Reference<DepthStencilTexture> depthTexture_) {
	baseTexture = baseTexture_;
	depthTexture = depthTexture_;
}

void OutlinePipeline::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 0, 1);
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 1, 1);
	rootSignatureBuilder.sampler(
		D3D12_SHADER_VISIBILITY_PIXEL,
		0
	);
	rootSignatureBuilder.sampler(
		D3D12_SHADER_VISIBILITY_PIXEL,
		1, 0,
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate();
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "FullscreenShader.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "Outline.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());
}
