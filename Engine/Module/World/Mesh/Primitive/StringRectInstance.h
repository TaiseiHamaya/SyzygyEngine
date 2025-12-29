#pragma once

#include "./IPrimitiveInstance.h"
#include "./CharRectStruct.h"

#include "Engine/Assets/FontAtlasMSDF/FontAtlasMSDFAsset.h"

namespace szg {

struct StringData {
	r32 fontSize;
	Vector2 pivot{ CVector2::ZERO };
	Vector2 offset;
	u32 textureIndex;
};

struct CharRectData {
	u32 top;
	u32 left;
	u32 bottom;
	u32 right;
};

class StringRectInstance : public IPrimitiveInstance<StringData> {
#ifdef DEBUG_FEATURES_ENABLE
	friend class RemoteStringRectInstance;
#endif // DEBUG_FEATURES_ENABLE
public:
	StringRectInstance() noexcept;
	virtual ~StringRectInstance() noexcept;

	SZG_CLASS_MOVE_ONLY(StringRectInstance)

public:
	void initialize(const std::string& msdfFont, r32 fontSize_, const Vector2& pivot_ = CVector2::ZERO);

public:
	void set_font_size(r32 fontSize);
	r32 font_size() const;
	r32 font_scale() const;

	void set_font(const std::string& fontName);

	void set_pivot(const Vector2& pivot);
	const Vector2& pivot_imm() const;
	const Vector2& offset_imm() const;
	
	/// <summary>
	/// 文字列の再生成
	/// </summary>
	/// <param name="string_"></param>
	void reset_string(std::string_view string_);
	void reset_string(std::wstring_view string_);

	/// <summary>
	/// 文字列を末尾に追加
	/// </summary>
	/// <param name="append_"></param>
	void append(const std::string& append_);
	void append(const std::wstring& append_);

	/// <summary>
	/// 文字を末尾に追加
	/// </summary>
	/// <param name="c"></param>
	void append(char c);
	void append(wchar_t c);

	void pop_back();

	/// <summary>
	/// 不変参照で文字列を取得
	/// </summary>
	/// <returns></returns>
	const std::string& string_imm() const;

	const std::vector<GlyphRenderingData>& glyph_data_imm() const;

	/// <summary>
	/// MSDF TextureのBindlessIndexの取得
	/// </summary>
	/// <returns></returns>
	u32 font_texture_index() const;

	/// <summary>
	/// GlyphのBindlessIndexの取得
	/// </summary>
	/// <returns></returns>
	std::optional<u32> glyph_bindless_index() const;

	std::shared_ptr<const FontAtlasMSDFAsset> font_atlas_asset() const noexcept;

private:
	std::shared_ptr<const FontAtlasMSDFAsset> fontAtlas;
	std::string string;
	std::vector<GlyphRenderingData> charRenderingData;

public:
	void debug_gui();
};

}; // szg
