#include "SkinningMeshDeferredPipeline.h"

#include "../DeferredAdaptor.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"
#include "Engine/GraphicsAPI/RenderingSystemValues.h"

using namespace szg;

SkinningMeshDeferredPipeline::SkinningMeshDeferredPipeline() = default;

SkinningMeshDeferredPipeline::~SkinningMeshDeferredPipeline() noexcept = default;

void SkinningMeshDeferredPipeline::initialize() {
	create_pipeline_state();
	pipelineState->set_name("SkinningMeshDeferredPipeline");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void SkinningMeshDeferredPipeline::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_VERTEX, 0, 1, 0); // 0 : transform(S0T0, V)
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_PIXEL, 0, 1, 0); // 1 : material(S0T0, P)
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_VERTEX, 0, 1); // 2 : camera proj(S1B0, V)
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_VERTEX, 1, 1, 0); // 3 : bone(S0T1, V)
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_VERTEX, 0, 0); // 4 : size of palette(S0B0, V)
	rootSignatureBuilder.sampler( // sampler
		D3D12_SHADER_VISIBILITY_PIXEL,
		0, 0,
		D3D12_FILTER_ANISOTROPIC
	);

	InputLayoutBuilder inputLayoutBuilder;
	inputLayoutBuilder.add_element("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0);
	inputLayoutBuilder.add_element("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0);
	inputLayoutBuilder.add_element("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0);

	inputLayoutBuilder.add_element("WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1);
	inputLayoutBuilder.add_element("INDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 1);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->depth_state(RenderingSystemValues::GetDepthStencilTexture()->get_as_dsv()->get_format());
	psoBuilder->inputlayout(inputLayoutBuilder.build());
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "SkinningMesh.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "Deferred.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->blendstate_only_write();
	for (u32 i = 0; i < DeferredAdaptor::NUM_GBUFFER; ++i) {
		psoBuilder->rendertarget(DeferredAdaptor::DXGI_FORMAT_LIST[i]);
	}

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());
}
