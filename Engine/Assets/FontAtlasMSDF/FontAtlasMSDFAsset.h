#pragma once

#include <string_view>
#include <unordered_map>
#include <vector>

#include <Library/Math/Matrix3x3.h>
#include <Library/Math/Vector2.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/GraphicsAPI/DirectX/DxResource/StructuredBuffer/StructuredBuffer.h"

namespace szg {

class TextureAsset;
struct GlyphRenderingData;

class FontAtlasMSDFAsset final {
public:
	struct GlyphDataGpu {
		Matrix3x3 uvMatrix;
		struct {
			r32 top;
			r32 left;
			r32 bottom;
			r32 right;
		} bounds;
	};

	struct GlyphData {
		r32 advance;
	};

	struct Data {
		r32 baseScale;
		r32 lineHeight;
	};

public:
	FontAtlasMSDFAsset(Data data, std::vector<std::pair<GlyphData, GlyphDataGpu>>&& glyphsData_, std::unordered_map<u32, u32>&& glyphMap_, std::shared_ptr<const TextureAsset> texture_);
	~FontAtlasMSDFAsset();

	SZG_CLASS_MOVE_ONLY(FontAtlasMSDFAsset)

public:
	const std::optional<u32>& glyph_bindless_index() const;
	std::optional<u32> texture_bindless_index() const;
	r32 base_scale() const;
	r32 line_height() const;

public:
	/// <summary>
	/// 文字列から描画用データを返す
	/// </summary>
	/// <param name="string">描画する文字列</param>
	/// <param name="fontSize">フォントサイズ</param>
	/// <returns></returns>
	std::vector<GlyphRenderingData> calculate_glyph(std::string_view string, r32 fontSize) const;

	/// <summary>
	/// 文字列の幅を計算する
	/// </summary>
	/// <param name="glyph"></param>
	/// <returns></returns>
	r32 calculate_advance(const std::vector<GlyphRenderingData>& glyph) const;

	/// <summary>
	/// 描画のオフセットを計算する
	/// </summary>
	/// <param name="glyph">文字列データ</param>
	/// <param name="pivot"></param>
	/// <param name="fontSize"></param>
	/// <returns></returns>
	Vector2 calculate_offset(const std::vector<GlyphRenderingData>& glyph, const Vector2& pivot, r32 fontSize) const;

private:
	Data data;

	std::vector<GlyphData> glyphsData;
	StructuredBuffer<GlyphDataGpu> glyphBuffers;

	std::shared_ptr<const TextureAsset> textureAsset;

	std::unordered_map<u32, u32> glyphMap;
};

}; // szg
