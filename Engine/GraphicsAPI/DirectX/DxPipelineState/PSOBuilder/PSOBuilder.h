#pragma once

#include <list>
#include <string>
#include <vector>

#include <d3d12.h>
#include <wrl/client.h>

#include "Engine/GraphicsAPI/DirectX/DxSystemValues.h"

namespace szg {

class DepthStencilView;
class DxShaderReflection;

class InputLayoutBuilder {
public:
	void add_element(string_literal semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT slot = 0);
	const std::vector<D3D12_INPUT_ELEMENT_DESC>& build();

private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
};

class RootSignatureBuilder {
public:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> build();

public:
	void add_cbv(D3D12_SHADER_VISIBILITY visibility, UINT shaderRegister, UINT space = 0);
	void add_structured(D3D12_SHADER_VISIBILITY visibility, UINT baseShaderRegister = 0, UINT numDescriptors = 1, UINT space = 0);
	void add_texture(D3D12_SHADER_VISIBILITY visibility, UINT baseShaderRegister = 0, UINT numDescriptors = 1, UINT space = 0);
	void sampler(
		D3D12_SHADER_VISIBILITY visibility,
		UINT shaderRegister,
		UINT space = 0,
		D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE textureMore = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_COMPARISON_FUNC func = D3D12_COMPARISON_FUNC_NEVER
	);

private:
	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	std::list<D3D12_DESCRIPTOR_RANGE> descriptorRanges;
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;
};

enum class PsoBlendMode : u32 {
	None,
	Alpha,
	Add,
	Subtract,
	Multily,
	Screen,
	LightingPath,
};

enum class ShaderType {
	Vertex,
	Pixel,
};

class PSOBuilder {
public:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> build();
	Microsoft::WRL::ComPtr<ID3D12RootSignature> get_rootsignature();

public:
	void initialize_by_reflection(const DxShaderReflection& reflection);

public:
	void rootsignature(const Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature_);
	void inputlayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& layout);
	void shaders(ShaderType type, const std::string& shaderFilename);
	void blendstate(PsoBlendMode blendMode = PsoBlendMode::None, u32 renderTarget = 0);
	void blendstate_only_write();
	void blendstate(D3D12_RENDER_TARGET_BLEND_DESC blendDesc, u32 renderTarget = 0);
	void rasterizerstate(D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK);
	void depth_state(DXGI_FORMAT format, D3D12_DEPTH_WRITE_MASK mask = D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC func = D3D12_COMPARISON_FUNC_LESS);
	void stencil_state(UINT8 read, D3D12_DEPTH_WRITE_MASK write);
	void front_face(D3D12_COMPARISON_FUNC func, D3D12_STENCIL_OP depthFail, D3D12_STENCIL_OP stencilFail, D3D12_STENCIL_OP stencilPass);
	void back_face(D3D12_COMPARISON_FUNC func, D3D12_STENCIL_OP depthFail, D3D12_STENCIL_OP stencilFail, D3D12_STENCIL_OP stencilPass);
	void primitivetopologytype(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE); // 使用するトポロジーのタイプ
	void rendertarget(DXGI_FORMAT format = DxSystemValues::SCREEN_RTV_FORMAT);

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
};

}; // szg
