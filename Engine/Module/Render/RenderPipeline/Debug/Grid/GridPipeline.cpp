#include "GridPipeline.h"

using namespace szg;

#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"
#include "Engine/GraphicsAPI/RenderingSystemValues.h"

void szg::GridPipeline::initialize() {
	create_pipeline_state();
	pipelineState->set_name("GridPipeline");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
}

void szg::GridPipeline::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_VERTEX, 0); // 1 : data
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_VERTEX, 1); // 2 : camera

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate();
	psoBuilder->depth_state(
		RenderingSystemValues::GetDepthStencilTexture()->get_as_dsv()->get_format(),
		D3D12_DEPTH_WRITE_MASK_ZERO
	);
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "Grid.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "Grid.PS.hlsl");
	psoBuilder->primitivetopologytype(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());

}
