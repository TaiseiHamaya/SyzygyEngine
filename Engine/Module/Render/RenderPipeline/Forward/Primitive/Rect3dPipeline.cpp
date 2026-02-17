#include "Rect3dPipeline.h"

#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"
#include "Engine/GraphicsAPI/RenderingSystemValues.h"

using namespace szg;

Rect3dPipeline::Rect3dPipeline() = default;
Rect3dPipeline::~Rect3dPipeline() noexcept = default;

void Rect3dPipeline::initialize() {
	initialize(BlendMode::Alpha);
}

void Rect3dPipeline::initialize(BlendMode blendMode) {
	create_pipeline_state(static_cast<PsoBlendMode>(blendMode));
	pipelineState->set_name("Rect3dPipeline");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void Rect3dPipeline::create_pipeline_state(PsoBlendMode blendMode) {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_VERTEX, 0, 1, 0); // 0 : transform(S0T0, V)
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_VERTEX, 1, 1, 0); // 1 : rect data(S0T1, V)
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_PIXEL, 0, 1, 0); // 2 : material(S0T0, P)
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_VERTEX, 0, 1); // 3 : camera vs(S1B0, V)
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_PIXEL, 0, 1); // 4 : camera ps(S1B0, P)
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_PIXEL, 0, 1, 2); // 5 : light(S1B0, P)
	rootSignatureBuilder.sampler( // sampler
		D3D12_SHADER_VISIBILITY_PIXEL,
		0, 0,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->depth_state(
		RenderingSystemValues::GetDepthStencilTexture()->get_as_dsv()->get_format(),
		blendMode == PsoBlendMode::None ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO
	);
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "Rect3d.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "ForwardAlpha.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->blendstate(blendMode);
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());
}
