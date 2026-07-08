#pragma once

#include "../IAssetBuilder.h"

#include "./FontAtlasMSDFAsset.h"

#include <filesystem>
#include <unordered_map>
#include <utility>
#include <vector>

namespace szg {

class TextureAssetBuilder;

class FontAtlasMSDFBuilder final : public IAssetBuilder {
public:
	FontAtlasMSDFBuilder(const std::filesystem::path& filePath);
	~FontAtlasMSDFBuilder();

public:
	bool run() override;

	void postprocess() override;

	void transfer() override;

private:
	std::unordered_map<u32, u32> glyphMap;
	std::vector<std::pair<FontAtlasMSDFAsset::GlyphData, FontAtlasMSDFAsset::GlyphDataGpu>> glyphsDataBuffer;
	FontAtlasMSDFAsset::Data data{};

	std::string ddsTextureName;

	std::shared_ptr<FontAtlasMSDFAsset> fontAtlas; // 実データ
	std::unique_ptr<TextureAssetBuilder> textureBuilder; // SDFテクスチャ読み込み用
};

}; // szg
