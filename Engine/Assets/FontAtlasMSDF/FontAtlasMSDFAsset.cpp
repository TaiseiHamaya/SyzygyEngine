#include "FontAtlasMSDFAsset.h"

using namespace szg;

#include <Library/Externals/msdf-atlas-gen/msdf-atlas-gen/msdf-atlas-gen.h>

#include "Engine/Assets/Texture/TextureAsset.h"
#include "Engine/Module/World/Mesh/Primitive/CharRectStruct.h"

FontAtlasMSDFAsset::FontAtlasMSDFAsset(Data data_, std::vector<std::pair<GlyphData, GlyphDataGpu>>&& glyphsData_, std::unordered_map<u32, u32>&& glyphMap_, std::shared_ptr<const TextureAsset> texture_) :
	data(data_),
	glyphMap(std::move(glyphMap_)),
	textureAsset(texture_) {
	glyphBuffers.initialize(static_cast<u32>(glyphsData_.size()));
	glyphsData.resize(glyphsData_.size());
	for (u32 i = 0; i < static_cast<u32>(glyphsData_.size()); ++i) {
		glyphsData[i] = glyphsData_[i].first;
		glyphBuffers[i] = glyphsData_[i].second;
	}
}

FontAtlasMSDFAsset::~FontAtlasMSDFAsset() = default;

const std::optional<u32>& FontAtlasMSDFAsset::glyph_bindless_index() const {
	return glyphBuffers.index();
}

std::optional<u32> FontAtlasMSDFAsset::texture_bindless_index() const {
	if (!textureAsset) {
		return std::nullopt;
	}
	return textureAsset->index();
}

r32 FontAtlasMSDFAsset::base_scale() const {
	return data.baseScale;
}

r32 FontAtlasMSDFAsset::line_height() const {
	return data.lineHeight;
}

std::vector<GlyphRenderingData> FontAtlasMSDFAsset::calculate_glyph(std::string_view string, r32 fontSize) const {
	std::vector<u32> codepoints;
	msdf_atlas::utf8Decode(codepoints, string.data());

	std::vector<u32> indices;
	for (u32& codepoint : codepoints) {
		// 改行文字の場合
		if (codepoint == 10) {
			indices.emplace_back(std::numeric_limits<u32>::max());
		}
		else if (glyphMap.contains(codepoint)) {
			indices.emplace_back(glyphMap.at(codepoint));
		}
		else {
			indices.emplace_back(0); // 未登録文字は0番にフォールバック
		}
	}
	std::vector<GlyphRenderingData> result(indices.size());
	r32 advancedX = 0.0f;
	r32 offsetY = 0.0f;
	for (i32 i = 0; i < static_cast<i32>(codepoints.size()); ++i) {
		u32 index = indices[i];

		// 改行文字
		if (index == std::numeric_limits<u32>::max()) {
			advancedX = 0.0f;
			offsetY += fontSize / data.baseScale * data.lineHeight;
			continue;
		}

		const GlyphDataGpu& glyphBuffer = glyphBuffers[index];
		const GlyphData& glyphData = glyphsData[index];

		GlyphRenderingData& write = result[i];
		write.glyphIndex = index;
		write.bottomLeft = Vector2{
			advancedX,
			glyphBuffer.bounds.bottom - offsetY
		};
		write.size = Vector2{
			glyphBuffer.bounds.right - glyphBuffer.bounds.left,
			glyphBuffer.bounds.top - glyphBuffer.bounds.bottom
		};
		advancedX -= glyphData.advance;
	}

	return result;
}

r32 szg::FontAtlasMSDFAsset::calculate_advance(const std::vector<GlyphRenderingData>& glyph) const {
	r32 advanced = 0.0f;
	for (i32 i = 0; i < static_cast<i32>(glyph.size()); ++i) {
		u32 index = glyph[i].glyphIndex;
		const GlyphData& glyphData = glyphsData[index];
		advanced += glyphData.advance;
	}
	return advanced;
}

Vector2 FontAtlasMSDFAsset::calculate_offset(const std::vector<GlyphRenderingData>& glyph, const Vector2& pivot, r32 fontSize) const {
	r32 advancedX = calculate_advance(glyph);
	Vector2 base = { advancedX, fontSize / data.baseScale * data.lineHeight };
	return Vector2::Multiply(base, pivot);
}
