#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <filesystem>
#include <vector>

#include <FontGeometry.h>
#include <ImmediateAtlasGenerator.h>
#include <GlyphGeometry.h>

#include <msdf-atlas-gen/glyph-generators.h>
#include <msdf-atlas-gen/BitmapAtlasStorage.h>

namespace szg {

class FontAtlasBuilder final {
public:
	void entry_point(const std::filesystem::path& inputFile, const std::filesystem::path& outputPath);

private:
	/// <summary>
	/// フォントデータの読み込み
	/// </summary>
	void load_glyphs();

	/// <summary>
	/// mtsdfテクスチャをdds形式で保存
	/// </summary>
	void save_mtsdf_texture(const std::filesystem::path& outputPath);

	/// <summary>
	/// mtsdfテクスチャのatlasデータの保存
	/// </summary>
	void save_atlas_data(const std::filesystem::path& outputPath);

private:
	std::filesystem::path ttfFilePath;

	msdf_atlas::ImmediateAtlasGenerator<float, 4, msdf_atlas::mtsdfGenerator, msdf_atlas::BitmapAtlasStorage<u8, 4>> generator;
	msdf_atlas::FontGeometry fontGeometry{};
	std::vector<msdf_atlas::GlyphGeometry> glyphs{};

	i32 textureWidth = 0;
	i32 textureHeight = 0;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
