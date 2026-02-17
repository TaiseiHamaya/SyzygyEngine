#include "ParticleBillboardPipeline.h"

#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"
#include "Engine/GraphicsAPI/RenderingSystemValues.h"

using namespace szg;

ParticleBillboardPipeline::ParticleBillboardPipeline() = default;

ParticleBillboardPipeline::~ParticleBillboardPipeline() noexcept = default;

void ParticleBillboardPipeline::initialize() {
	create_pipeline_state();
	pipelineState->set_name("ParticleBillboardPipeline");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void ParticleBillboardPipeline::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_VERTEX, 0, 1); // 0 :  transform
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_VERTEX, 0); // 1 : camera
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 0); // 2 : texture
	rootSignatureBuilder.sampler( // sampler
		D3D12_SHADER_VISIBILITY_PIXEL,
		0, 0,
		D3D12_FILTER_ANISOTROPIC
	);

	InputLayoutBuilder inputLayoutBuilder;
	inputLayoutBuilder.add_element("POSITION", 0, DXGI_FORMAT_R32G32_FLOAT);
	inputLayoutBuilder.add_element("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate(PsoBlendMode::Alpha);
	psoBuilder->depth_state(RenderingSystemValues::GetDepthStencilTexture()->get_as_dsv()->get_format(), D3D12_DEPTH_WRITE_MASK_ZERO);
	psoBuilder->inputlayout(inputLayoutBuilder.build());
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "ParticleBillboard.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "ParticleBillboard.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());

}
