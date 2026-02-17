#include "ShaderAsset.h"

using namespace szg;

ShaderAsset::ShaderAsset(Microsoft::WRL::ComPtr<IDxcBlob> blob_) noexcept :
	blob(blob_) {
}

D3D12_SHADER_BYTECODE ShaderAsset::blob_bytecode() const noexcept {
	return { blob->GetBufferPointer(), blob->GetBufferSize() };
}

DxcBuffer ShaderAsset::blob_buffer() const noexcept {
	return { blob->GetBufferPointer(), blob->GetBufferSize(), 0 };
}
