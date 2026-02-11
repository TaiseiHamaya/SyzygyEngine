#include "NonLightingPixelPipeline.h"

using namespace szg;

#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"
#include "Engine/Module/Render/RenderTargetGroup/MultiRenderTarget.h"

NonLightingPixelPipeline::NonLightingPixelPipeline() = default;
NonLightingPixelPipeline::~NonLightingPixelPipeline() noexcept = default;

void NonLightingPixelPipeline::initialize() {
	create_pipeline_state();
	pipelineState->set_name("NonLightingPixelPipeline");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void NonLightingPixelPipeline::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 0); // 0 : Albedo

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate(PsoBlendMode::None);
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "FullscreenShader.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "NonLighting.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());
}
