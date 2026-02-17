#include "DxShaderReflection.h"

#pragma warning(push, 0)

#include "./DxcManager.h"
#include "Engine/GraphicsAPI/DirectX/DxPipelineState/PSOBuilder/PSOBuilder.h"

using namespace szg;

//void DxShaderReflection::initialize(const ShaderBuilder& shader) {
//	DxcBuffer vsBuffer = shader.vs_buffer();
//	DxcManager::GetUtils().CreateReflection(
//		&vsBuffer,
//		IID_PPV_ARGS(&vsReflector)
//	);
//
//	DxcBuffer psBuffer = shader.ps_buffer();
//	DxcManager::GetUtils().CreateReflection(
//		&psBuffer,
//		IID_PPV_ARGS(&psReflector)
//	);
//
//	D3D12_SHADER_DESC shaderDesc;
//	vsReflector->GetDesc(&shaderDesc);
//
//	// ---------- InputLayout構築 ----------
//	InputLayoutBuilder inputLayout{};
//	for (UINT i = 0; i < shaderDesc.InputParameters; ++i) {
//		D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
//		vsReflector->GetInputParameterDesc(i, &paramDesc);
//
//		inputLayout.add_element(
//			paramDesc.SemanticName,
//			paramDesc.SemanticIndex,
//			guess_format(paramDesc)
//		);
//	}
//	inputElementDescs = inputLayout.build();
//}

Microsoft::WRL::ComPtr<ID3D12RootSignature> DxShaderReflection::build_root_signature(std::vector<SamplerDesc> descs) {
	RootSignatureBuilder rootSignatureBuilder;

	// ---------- VS ----------
	D3D12_SHADER_DESC vsDesc;
	vsReflector->GetDesc(&vsDesc);
	for (UINT i = 0; i < vsDesc.BoundResources; ++i) {
		D3D12_SHADER_INPUT_BIND_DESC bindDesc;
		vsReflector->GetResourceBindingDesc(i, &bindDesc);
		guess_bind(rootSignatureBuilder, D3D12_SHADER_VISIBILITY_VERTEX, bindDesc);
	}

	// ---------- PS ----------
	D3D12_SHADER_DESC psDesc;
	vsReflector->GetDesc(&psDesc);
	for (UINT i = 0; i < psDesc.BoundResources; ++i) {
		D3D12_SHADER_INPUT_BIND_DESC bindDesc;
		vsReflector->GetResourceBindingDesc(i, &bindDesc);
		guess_bind(rootSignatureBuilder, D3D12_SHADER_VISIBILITY_PIXEL, bindDesc);
	}

	return rootSignatureBuilder.build();
}

DXGI_FORMAT DxShaderReflection::guess_format(const D3D12_SIGNATURE_PARAMETER_DESC& desc) {
	if (desc.Mask == 1) {
		if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  return DXGI_FORMAT_R32_UINT;
		if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  return DXGI_FORMAT_R32_SINT;
		if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32_FLOAT;
	}
	else if (desc.Mask <= 3) {
		if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  return DXGI_FORMAT_R32G32_UINT;
		if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  return DXGI_FORMAT_R32G32_SINT;
		if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32_FLOAT;
	}
	else if (desc.Mask <= 7) {
		if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  return DXGI_FORMAT_R32G32B32_UINT;
		if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  return DXGI_FORMAT_R32G32B32_SINT;
		if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32B32_FLOAT;
	}
	else if (desc.Mask <= 15) {
		if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  return DXGI_FORMAT_R32G32B32A32_UINT;
		if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  return DXGI_FORMAT_R32G32B32A32_SINT;
		if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}

	return DXGI_FORMAT_UNKNOWN;
}

void DxShaderReflection::guess_bind(RootSignatureBuilder& rootSignatureBuilder, D3D12_SHADER_VISIBILITY visibility, const D3D12_SHADER_INPUT_BIND_DESC& bindDesc) {

	switch (bindDesc.Type) {
	case D3D_SIT_CBUFFER:
		rootSignatureBuilder.add_cbv(visibility, bindDesc.BindPoint, bindDesc.Space);
		break;

	case D3D_SIT_TEXTURE:
		rootSignatureBuilder.add_texture(visibility, bindDesc.BindPoint, bindDesc.BindCount, bindDesc.Space);
		break;

	case D3D_SIT_STRUCTURED:
		rootSignatureBuilder.add_structured(visibility, bindDesc.BindPoint, bindDesc.BindCount, bindDesc.Space);
		break;

	case D3D_SIT_SAMPLER:
		rootSignatureBuilder.sampler(visibility, bindDesc.BindPoint, bindDesc.Space);
		break;

	default:
		break;
	}
}

#pragma warning(pop)
