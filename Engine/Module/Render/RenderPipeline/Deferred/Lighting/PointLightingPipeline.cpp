#include "PointLightingPipeline.h"

using namespace szg;

#include "Engine/GraphicsAPI/DirectX/DxPipelineState/DxPipelineState.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"
#include "Engine/GraphicsAPI/RenderingSystemValues.h"
#include "Engine/Module/Render/RenderTargetGroup/MultiRenderTarget.h"

PointLightingPipeline::PointLightingPipeline() = default;

PointLightingPipeline::~PointLightingPipeline() noexcept = default;

void PointLightingPipeline::initialize() {
	create_pipeline_state();
	pipelineState->set_name("PointLightingPipeline");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void PointLightingPipeline::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_VERTEX, 0); // 0 : Transform
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_VERTEX, 0); // 1 : Camera
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 0); // 2 : Albedo
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 1); // 3 : Normal
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL, 2); // 4 : Depth
	rootSignatureBuilder.add_structured(D3D12_SHADER_VISIBILITY_PIXEL, 3); // 5 : Light
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_PIXEL, 0); // 6 : Camera
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_PIXEL, 1); // 7 : OutputSize

	InputLayoutBuilder inputLayoutBuilder;
	inputLayoutBuilder.add_element("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate(PsoBlendMode::LightingPath);
	psoBuilder->rasterizerstate(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_FRONT);
	psoBuilder->inputlayout(inputLayoutBuilder.build());
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(ShaderType::Vertex, "PrimitiveGeometry.VS.hlsl");
	psoBuilder->shaders(ShaderType::Pixel, "PointLighting.PS.hlsl");
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<DxPipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());
}
