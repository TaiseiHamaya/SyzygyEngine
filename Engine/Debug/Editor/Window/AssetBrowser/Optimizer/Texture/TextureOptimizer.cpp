#include "TextureOptimizer.h"

#include <Windows.h>

using namespace szg;

void TextureOptimizer::convert_wic_to_dds(const std::filesystem::path& inputFile, const std::filesystem::path& outputPath) {
	load_wic_from_file(inputFile);
	save_dds_to_file(outputPath / (inputFile.stem().native() + L".dds"));
}

void TextureOptimizer::load_wic_from_file(const std::filesystem::path& path) {
	HRESULT hr;
	hr = DirectX::LoadFromWICFile(path.native().c_str(), DirectX::WIC_FLAGS_NONE, &metadata, scratchImage);
	assert(SUCCEEDED(hr));
}

void TextureOptimizer::save_dds_to_file(const std::filesystem::path& path) {
	HRESULT hr;
	// MipMap生成
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
	assert(SUCCEEDED(hr));
	scratchImage = std::move(mipChain);

	// 圧縮
	DirectX::ScratchImage compressed;
	hr = DirectX::Compress(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(),
		DXGI_FORMAT_BC7_UNORM_SRGB, DirectX::TEX_COMPRESS_BC7_QUICK | DirectX::TEX_COMPRESS_SRGB_OUT | DirectX::TEX_COMPRESS_PARALLEL, 1.0f, compressed);
	assert(SUCCEEDED(hr));
	// 転送
	scratchImage = std::move(compressed);
	metadata = scratchImage.GetMetadata();

	metadata.format = DirectX::MakeSRGB(metadata.format);
	hr = DirectX::SaveToDDSFile(scratchImage.GetImages(), scratchImage.GetImageCount(), metadata, DirectX::DDS_FLAGS_NONE, path.native().c_str());
	assert(SUCCEEDED(hr));
}
