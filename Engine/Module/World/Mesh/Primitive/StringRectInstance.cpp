#include "StringRectInstance.h"

#include "Engine/Assets/FontAtlasMSDF/FontAtlasMSDFLibrary.h"

#include <Library/Utility/Tools/ConvertString.h>

using namespace szg;

StringRectInstance::StringRectInstance() noexcept = default;

StringRectInstance::~StringRectInstance() noexcept = default;

void StringRectInstance::initialize(const std::string& msdfFont, r32 fontSize_, const Vector2& pivot_) {
	data.fontSize = fontSize_;
	data.pivot = pivot_;
	fontAtlas = FontAtlasMSDFLibrary::Get(msdfFont);
}

void StringRectInstance::set_font_size(r32 fontSize) {
	data.fontSize = fontSize;
}

r32 StringRectInstance::font_size() const {
	return data.fontSize;
}

r32 StringRectInstance::font_scale() const {
	return data.fontSize / fontAtlas->base_scale();
}

void StringRectInstance::set_font(const std::string& fontName) {
	fontAtlas = FontAtlasMSDFLibrary::Get(fontName);
	if (!fontAtlas) {
		return;
	}
	charRenderingData = fontAtlas->calculate_glyph(string);
	data.offset = fontAtlas->calculate_offset(charRenderingData, data.pivot, data.fontSize);
}

void StringRectInstance::set_pivot(const Vector2& pivot) {
	data.pivot = pivot;
	if (!fontAtlas) {
		return;
	}
	data.offset = fontAtlas->calculate_offset(charRenderingData, data.pivot, data.fontSize);
}

const Vector2& StringRectInstance::pivot_imm() const {
	return data.pivot;
}

const Vector2& StringRectInstance::offset_imm() const {
	return data.offset;
}

void StringRectInstance::reset_string(std::string_view string_) {
	string = string_;
	charRenderingData.clear();
	if (!fontAtlas) {
		return;
	}
	charRenderingData = fontAtlas->calculate_glyph(string);
	data.offset = fontAtlas->calculate_offset(charRenderingData, data.pivot, data.fontSize);
}

void szg::StringRectInstance::reset_string(std::wstring_view string_) {
	reset_string(ConvertString(string_));
}

const std::string& StringRectInstance::string_imm() const {
	return string;
}

void StringRectInstance::append(const std::string& append_) {
	reset_string(string + append_);
}

void StringRectInstance::append(const std::wstring& append_) {
	append(ConvertString(append_));
}

void StringRectInstance::append(char c) {
	reset_string(string + c);
}

void StringRectInstance::append(wchar_t c) {
	reset_string(string + ConvertString(std::wstring(1, c)));
}

void szg::StringRectInstance::pop_back() {
	if (string.empty()) {
		//szgWarning()
		return;
	}
	string.pop_back();
	charRenderingData = fontAtlas->calculate_glyph(string);
	data.offset = fontAtlas->calculate_offset(charRenderingData, data.pivot, data.fontSize);
}

const std::vector<GlyphRenderingData>& StringRectInstance::glyph_data_imm() const {
	return charRenderingData;
}

u32 StringRectInstance::font_texture_index() const {
	if (!fontAtlas) {
		return 0;
	}
	auto textureIndexOpt = fontAtlas->texture_bindless_index();
	if (!textureIndexOpt.has_value()) {
		return 0;
	}
	return textureIndexOpt.value();
}

std::optional<u32> StringRectInstance::glyph_bindless_index() const {
	if (!fontAtlas) {
		return std::nullopt;
	}
	return fontAtlas->glyph_bindless_index();
}

std::shared_ptr<const FontAtlasMSDFAsset> szg::StringRectInstance::font_atlas_asset() const noexcept {
	return fontAtlas;
}
