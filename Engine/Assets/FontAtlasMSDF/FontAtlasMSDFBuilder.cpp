#include "FontAtlasMSDFBuilder.h"

using namespace szg;

#include <Library/Math/Vector2.h>
#include <Library/Math/Transform2D.h>

#include "../Texture/TextureAssetBuilder.h"
#include "../Texture/TextureLibrary.h"
#include "./FontAtlasMSDFLibrary.h"
#include "Engine/Assets/Json/JsonAsset.h"

#define VECTOR2_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

FontAtlasMSDFBuilder::FontAtlasMSDFBuilder(const std::filesystem::path& filePath_) {
	filePath = IAssetBuilder::ResolveFilePath(filePath_, "FontAtlasMSDF");
}

FontAtlasMSDFBuilder::~FontAtlasMSDFBuilder() = default;

bool FontAtlasMSDFBuilder::run() {
	szgInformation(L"Start load font mtsdf atlas file-\'{}\'", filePath.native());

	JsonAsset json;
	json.load(filePath);
	nlohmann::json glyphsJson = json.get()["Glyphs"];

	if (glyphsJson.empty()) {
		return false;
	}

	glyphsDataBuffer.resize(glyphsJson.size());

	for (i32 i = 0; auto& glyphJson : glyphsJson) {
		FontAtlasMSDFAsset::GlyphDataGpu glyphBuffer;
		FontAtlasMSDFAsset::GlyphData glyphData;
		// テクスチャUV
		const nlohmann::json& textureJson = glyphJson["Texture"];
		Vector2 scale = textureJson.value("Scale", CVector2::ONE);
		Vector2 translate = textureJson.value("Translate", CVector2::ZERO);
		glyphBuffer.uvMatrix = Transform2D::MakeAffineMatrix(scale, 0.0f, translate);
		// 文字の矩形
		const nlohmann::json& boundsJson = glyphJson["BoundsBox"];
		glyphBuffer.bounds.top = boundsJson.value("Top", 0.0f);
		glyphBuffer.bounds.left = boundsJson.value("Left", 0.0f);
		glyphBuffer.bounds.bottom = boundsJson.value("Bottom", 0.0f);
		glyphBuffer.bounds.right = boundsJson.value("Right", 0.0f);
		// 文字列にする際に使う
		glyphData.advance = glyphJson.value("Advance", 0.0f);
		// Codepoint
		u32 codepoint = glyphJson["Codepoint"].get<u32>();
		// 重複登録防止
		if (glyphMap.contains(codepoint)) {
			continue;
		}
		// 書き込み
		glyphsDataBuffer[i] = { glyphData, glyphBuffer };
		glyphMap[codepoint] = i;

		++i;
	}

	data.baseScale = json.get().value("BaseFontScale", 1.0f);
	data.lineHeight = json.get().value("LineHeight", 0.0f);
	ddsTextureName = json.get().value("DDSTexture", filePath.stem().string() + ".dds");

	textureBuilder = std::make_unique<TextureAssetBuilder>(filePath.parent_path() / ddsTextureName);
	if (!textureBuilder->run()) {
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
