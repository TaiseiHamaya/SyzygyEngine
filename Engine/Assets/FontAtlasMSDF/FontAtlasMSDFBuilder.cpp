#include "FontAtlasMSDFBuilder.h"

#include <Library/Math/Transform2D.h>
#include <Library/Math/Vector2.h>

#include "../Texture/TextureAssetBuilder.h"
#include "../Texture/TextureLibrary.h"
#include "./FontAtlasMSDFLibrary.h"
#include "Engine/Assets/Json/JsonAsset.h"

#define VECTOR2_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

using namespace szg;

FontAtlasMSDFBuilder::FontAtlasMSDFBuilder(const std::filesystem::path& filePath_) {
	filePath = IAssetBuilder::ResolveFilePath(filePath_, "FontAtlasMSDF");
}

FontAtlasMSDFBuilder::~FontAtlasMSDFBuilder() = default;

bool FontAtlasMSDFBuilder::run() {
	szgInformation(L"Start load font mtsdf atlas. File-\'{}\'", filePath.native());

	JsonAsset json;
	json.load(filePath);

	if (json.cget().is_null()) {
		// mtsdfフォントの定義ファイルが見つからない or 内容が正しくない
		szgWarning(L"Failed to load mtsdf file. File-\'{}\'", filePath.native());
		return false;
	}

	u32 textureWidth{};
	u32 textureHeight{};
	{
		const nlohmann::json& atlasJson = json.get()["atlas"];
		if (atlasJson.empty()) {
			szgWarning(L"Failed to load mtsdf file. File-\'{}\', Reason-\'Atlas data is empty.\'", filePath.native());
			return false;
		}
		textureWidth = atlasJson.value("width", 0);
		textureHeight = atlasJson.value("height", 0);
		ddsTextureName = atlasJson.value("texture", "");
	}

	{
		const nlohmann::json& atlasJson = json.get()["metrics"];
		if (atlasJson.empty()) {
			szgWarning(L"Failed to load mtsdf file. File-\'{}\', Reason-\'Metrics data is empty.\'", filePath.native());
			return false;
		}
		data.baseScale = atlasJson.value("emSize", 1.0f);
		data.lineHeight = atlasJson.value("lineHeight", 0.0f);
		data.ascender = atlasJson.value("ascender", 0.0f);
		data.descender = atlasJson.value("descender", 0.0f);
	}

	// 文字データ
	nlohmann::json glyphsJson = json.get()["glyphs"];
	glyphsDataBuffer.resize(glyphsJson.size());
	if (glyphsJson.empty()) {
		szgWarning(L"Failed to load mtsdf file. File-\'{}\', Reason-\'Glyphs data is empty.\'", filePath.native());
		return false;
	}
	for (i32 i = 0; auto& glyphJson : glyphsJson) {
		FontAtlasMSDFAsset::GlyphDataGpu glyphBuffer;
		FontAtlasMSDFAsset::GlyphData glyphData;
		u32 unicode = glyphJson["unicode"].get<u32>();
		glyphData.advance = glyphJson.value("advance", 0.0f);

		{	// テクスチャUV
			if (glyphJson.contains("atlasBounds")) {
				const nlohmann::json& atlasBounsJson = glyphJson["atlasBounds"];
				r32 bottom = atlasBounsJson.value("bottom", 0.0f) / textureHeight;
				r32 left = atlasBounsJson.value("left", 0.0f) / textureWidth;
				r32 top = atlasBounsJson.value("top", 0.0f) / textureHeight;
				r32 right = atlasBounsJson.value("right", 0.0f) / textureWidth;

				Vector2 scale = Vector2{ right - left, top - bottom };
				Vector2 translate = { left, bottom };
				glyphBuffer.uvMatrix = Transform2D::MakeAffineMatrix(scale, 0.0f, translate);
			}
		}

		{	// 文字の矩形
			if (glyphJson.contains("planeBounds")) {
				const nlohmann::json& planeBoundsJson = glyphJson["planeBounds"];
				r32 bottom = planeBoundsJson.value("bottom", 0.0f);
				r32 left = planeBoundsJson.value("left", 0.0f);
				r32 top = planeBoundsJson.value("top", 0.0f);
				r32 right = planeBoundsJson.value("right", 0.0f);

				glyphBuffer.bounds.top = top;
				glyphBuffer.bounds.bottom = bottom;
				glyphBuffer.bounds.left = left;
				glyphBuffer.bounds.right = right;
			}
		}
		// 重複登録防止
		if (glyphMap.contains(unicode)) {
			continue;
		}
		// 書き込み
		glyphsDataBuffer[i] = { glyphData, glyphBuffer };
		glyphMap[unicode] = i;

		++i;
	}

	textureBuilder = std::make_unique<TextureAssetBuilder>(filePath.parent_path() / ddsTextureName);
	if (!textureBuilder->run()) {
		// テクスチャのロードに失敗
		return false;
	}
	return true;
}

void FontAtlasMSDFBuilder::postprocess() {
	textureBuilder->postprocess();
	auto texture = textureBuilder->texture_data();
	fontAtlas = std::make_shared<FontAtlasMSDFAsset>(
		data,
		std::move(glyphsDataBuffer),
		std::move(glyphMap),
		texture
	);
}

void FontAtlasMSDFBuilder::transfer() {
	// MTSDFテクスチャ転送
	textureBuilder->transfer();
	// MSDF Atlasの転送
	FontAtlasMSDFLibrary::Transfer(filePath.filename().string(), fontAtlas);
}
