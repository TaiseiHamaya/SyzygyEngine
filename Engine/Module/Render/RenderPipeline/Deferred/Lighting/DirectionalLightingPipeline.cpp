#include "DirectionalLightingPipeline.h"

using namespace szg;

#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"
#include "Engine/GraphicsAPI/RenderingSystemValues.h"
#include "Engine/Module/Render/RenderTargetGroup/MultiRenderTarget.h"

DirectionalLightingPipeline::DirectionalLightingPipeline() = default;

DirectionalLightingPipeline::~DirectionalLightingPipeline() noexcept = default;

void DirectionalLightingPipeline::initialize() {
	create_pipeline_state();
	pipelineState->set_name("DirectionalLightingPipeline");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void DirectionalLightingPipeline::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_PIXEL, 3); // 0 : Light
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_PIXEL, 0); // 1 : Camera
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 0); // 2 : Albedo
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 1); // 3 : Normal
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 2); // 4 : Depth

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate(PsoBlendMode::LightingPath);
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "FullscreenShader.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "DirectionalLighting.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());
}
