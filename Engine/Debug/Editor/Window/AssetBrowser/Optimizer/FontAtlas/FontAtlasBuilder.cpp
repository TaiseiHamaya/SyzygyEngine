#ifdef DEBUG_FEATURES_ENABLE

#include "FontAtlasBuilder.h"

#include <thread>

#include "./FontAtlasBuilderManager.h"
#include "Engine/Assets/Json/JsonAsset.h"

using namespace msdf_atlas;

using namespace szg;

void FontAtlasBuilder::entry_point(const std::filesystem::path& inputFile, const std::filesystem::path& outputPath) {
	ttfFilePath = inputFile;

	// ロード
	load_glyphs();

	save_mtsdf_texture(outputPath);
	save_atlas_data(outputPath);
}

void FontAtlasBuilder::load_glyphs() {
	msdfgen::FontHandle* font = loadFont(FontAtlasBuilderManager::FreetypeHandle(), ttfFilePath.string().c_str());
	if (!font) {
		return;
	}

	fontGeometry = FontGeometry{ &glyphs };
	fontGeometry.loadCharset(font, 1.0f, FontAtlasBuilderManager::Charset());
	for (GlyphGeometry& glyph : glyphs) {
		glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, 3.0, 0);
	}

	TightAtlasPacker packer;
	packer.setMinimumScale(10.0);
	packer.setPixelRange(2.0);
	packer.setUnitRange(1.0);
	packer.setDimensionsConstraint(DimensionsConstraint::POWER_OF_TWO_SQUARE);
	packer.pack(glyphs.data(), static_cast<int>(glyphs.size()));

	packer.getDimensions(textureWidth, textureHeight);

	// 実際の生成
	generator.resize(textureWidth, textureHeight);
	GeneratorAttributes attributes;
	generator.setAttributes(attributes);
	generator.setThreadCount(std::thread::hardware_concurrency());
	generator.generate(glyphs.data(), static_cast<int>(glyphs.size()));

	// 削除
	destroyFont(font);
}

void FontAtlasBuilder::save_mtsdf_texture(const std::filesystem::path& outputPath) {
	msdfgen::BitmapConstRef<float, 4> bitmapRef = generator.atlasStorage();
	HRESULT hr;
	DirectX::Image image = {}; // 書き込み
	image.width = bitmapRef.width;
	image.height = bitmapRef.height;
	image.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	image.rowPitch = sizeof(float) * 4 * bitmapRef.width;
	image.slicePitch = image.rowPitch * bitmapRef.height;
	image.pixels = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(bitmapRef.pixels));

	// ScratchImage に変換
	DirectX::ScratchImage scratchImage;
	hr = scratchImage.InitializeFromImage(image);
	assert(SUCCEEDED(hr));

	// 圧縮
	//DirectX::ScratchImage compressed;
	//hr = DirectX::Compress(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(),
	//	DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_BC7_QUICK | DirectX::TEX_COMPRESS_PARALLEL, 1.0f, compressed);
	//assert(SUCCEEDED(hr));

	// 転送
	//scratchImage = std::move(compressed);
	DirectX::TexMetadata metadata = scratchImage.GetMetadata();

	// 保存
	std::filesystem::path outputFilePath = outputPath.parent_path() / (ttfFilePath.stem().native() + L".dds");
	hr = DirectX::SaveToDDSFile(scratchImage.GetImages(), scratchImage.GetImageCount(), metadata, DirectX::DDS_FLAGS_NONE, outputFilePath.c_str());
	assert(SUCCEEDED(hr));
}

void FontAtlasBuilder::save_atlas_data(const std::filesystem::path& outputPath) {
	std::filesystem::path outputFilePath = outputPath / (ttfFilePath.stem().native() + L".mtsdf");

	JsonAtlasMetrics jsonMetrics = {};
	jsonMetrics.distanceRange = 2;
	jsonMetrics.width = textureWidth;
	jsonMetrics.height = textureHeight;
	jsonMetrics.yDirection = YDirection::BOTTOM_UP;
	jsonMetrics.size = 1.0;

	msdf_atlas::exportJSON(&fontGeometry, 1, ImageType::MTSDF, jsonMetrics, outputFilePath.string().c_str(), true);

	JsonAsset json;
	json.load(outputFilePath);

	json.get()["atlas"]["texture"] = ttfFilePath.stem().string() + ".dds";
	json.save();
}

#endif // DEBUG_FEATURES_ENABLE
