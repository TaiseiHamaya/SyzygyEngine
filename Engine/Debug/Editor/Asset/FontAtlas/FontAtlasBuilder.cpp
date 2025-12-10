#ifdef DEBUG_FEATURES_ENABLE

#include "FontAtlasBuilder.h"

using namespace szg;

#include <thread>

#include <Library/Math/Vector2.h>

#include "./FontAtlasBuilderManager.h"
#include "Engine/Assets/Json/JsonAsset.h"

#define VECTOR2_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

using namespace msdf_atlas;

void FontAtlasBuilder::entry_point(const std::filesystem::path& path) {
	ttfFilePath = path;

	// ロード
	load_glyphs();

	save_mtsdf_texture();
	save_atlas_data();
}

void FontAtlasBuilder::load_glyphs() {
	msdfgen::FontHandle* font = loadFont(FontAtlasBuilderManager::FreetypeHandle(), ttfFilePath.string().c_str());
	if (!font) {
		return;
	}

	FontGeometry fontGeometry(&glyphs);
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

	baseFontScale = static_cast<r32>(packer.getScale());
	lineHeight = static_cast<r32>(fontGeometry.getMetrics().lineHeight);

	int sizeX, sizeY;
	packer.getDimensions(sizeX, sizeY);

	// 実際の生成
	generator.resize(sizeX, sizeY);
	GeneratorAttributes attributes;
	generator.setAttributes(attributes);
	generator.setThreadCount(std::thread::hardware_concurrency());
	generator.generate(glyphs.data(), static_cast<int>(glyphs.size()));

	// 削除
	destroyFont(font);
}

void FontAtlasBuilder::save_mtsdf_texture() {
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
	std::filesystem::path outputPath = ttfFilePath.parent_path() / (ttfFilePath.stem().native() + L".dds");
	hr = DirectX::SaveToDDSFile(scratchImage.GetImages(), scratchImage.GetImageCount(), metadata, DirectX::DDS_FLAGS_NONE, outputPath.c_str());
	assert(SUCCEEDED(hr));
}

void FontAtlasBuilder::save_atlas_data() {
	JsonAsset json;
	std::filesystem::path outputPath = ttfFilePath.parent_path() / (ttfFilePath.stem().native() + L".mtsdf");
	json.load(outputPath);
	json.get().clear();

	size_t textureWidth = scratchImage.GetMetadata().width;
	size_t textureHeight = scratchImage.GetMetadata().height;
	// Glyphの情報を保存
	nlohmann::json atlasJson = nlohmann::json::array();
	for (const GlyphGeometry& glyph : glyphs) {
		nlohmann::json glyphJson;
		glyphJson["Codepoint"] = glyph.getCodepoint();
		double lt, bt, rt, tt;
		glyph.getQuadAtlasBounds(lt, bt, rt, tt);

		nlohmann::json textureJson = nlohmann::json::object();
		textureJson["Scale"] = Vector2{
			static_cast<r32>(rt - lt) / textureWidth,
			static_cast<r32>(tt - bt) / textureHeight,
		};
		textureJson["Translate"] = Vector2{
			static_cast<r32>(lt) / textureWidth,
			static_cast<r32>(bt) / textureHeight,
		};
		glyphJson["Texture"] = textureJson;

		glyphJson["Index"] = glyph.getIndex();
		glyphJson["Advance"] = glyph.getAdvance();
		double lb, bb, rb, tb;
		glyph.getQuadPlaneBounds(lb, bb, rb, tb);
		glyphJson["BoundsBox"] = { {"Left", lb}, {"Bottom", bb}, {"Right", rb}, {"Top", tb} };
		atlasJson.push_back(glyphJson);
	}

	json.get()["Glyphs"] = atlasJson;

	json.get()["TextureWidth"] = textureWidth;
	json.get()["TextureHeight"] = textureHeight;

	json.get()["BaseFontScale"] = baseFontScale;
	json.get()["LineHeight"] = lineHeight;

	json.get()["DDSTexture"] = ttfFilePath.stem().string() + ".dds";

	json.save();
}

#endif // DEBUG_FEATURES_ENABLE
