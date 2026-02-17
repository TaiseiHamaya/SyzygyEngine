#include "TextureAsset.h"

#include <Library/Utility/Tools/ConvertString.h>

using namespace szg;

TextureAsset::TextureAsset(Microsoft::WRL::ComPtr<ID3D12Resource> resource, bool isCubemap) {
	texture.initialize(resource, isCubemap);
	if (texture.get_srv()->get_index().has_value()) {
		gpuIndex = texture.get_srv()->get_index().value();
	}
}

u32 TextureAsset::index() const {
	return gpuIndex;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureAsset::handle() const {
	return texture.get_srv()->handle();
}

Reference<const ShaderResourceView> TextureAsset::get_srv() const {
	return texture.get_srv();
}

#ifdef DEBUG_FEATURES_ENABLE

void TextureAsset::set_name(const std::wstring& _name) {
	name_ = ConvertString(_name);
	texture.set_name(_name);
}

const std::string& TextureAsset::name() const {
	return name_;
}

#endif // DEBUG_FEATURES_ENABLE
