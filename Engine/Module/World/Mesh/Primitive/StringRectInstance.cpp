#include "StringRectInstance.h"

using namespace szg;

#include "Engine/Assets/FontAtlasMSDF/FontAtlasMSDFLibrary.h"

#include <Library/Utility/Tools/ConvertString.h>

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
	charRenderingData = fontAtlas->calculate_glyph(string, data.fontSize);
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
	charRenderingData = fontAtlas->calculate_glyph(string, data.fontSize);
	data.offset = fontAtlas->calculate_offset(charRenderingData, data.pivot, data.fontSize);
}

void szg::StringRectInstance::reset_string(std::wstring_view string_) {
	std::string tempStr;
	tempStr.reserve(string_.size());
	tempStr = ConvertString(string_);
	string = tempStr;
	charRenderingData.clear();
	if (!fontAtlas) {
		return;
	}
	charRenderingData = fontAtlas->calculate_glyph(string, data.fontSize);
	data.offset = fontAtlas->calculate_offset(charRenderingData, data.pivot, data.fontSize);
}

const std::string& StringRectInstance::string_imm() const {
	return string;
}

void StringRectInstance::append(const std::string& append_) {
	auto temp = fontAtlas->calculate_glyph(append_, data.fontSize);
	data.offset += fontAtlas->calculate_offset(temp, data.pivot, data.fontSize);
	charRenderingData.insert(charRenderingData.end(), temp.begin(), temp.end());
	string += append_;
}

void StringRectInstance::append(const std::wstring& append_) {
	std::string tempStr;
	tempStr.reserve(append_.size());
	tempStr = ConvertString(append_);
	string += tempStr;
	// TODO: 差分だけ計算し直すみたいなことをしたい
	charRenderingData = fontAtlas->calculate_glyph(string, data.fontSize);
	data.offset = fontAtlas->calculate_offset(charRenderingData, data.pivot, data.fontSize);
}

void StringRectInstance::append(char c) {
	auto temp = fontAtlas->calculate_glyph(std::string(1, c), data.fontSize);
	data.offset += fontAtlas->calculate_offset(temp, data.pivot, data.fontSize);
	charRenderingData.emplace_back(temp[0]);
	string += c;
}

void StringRectInstance::append(wchar_t c) {
	std::string tempStr;
	tempStr = ConvertString(std::wstring(1, c));
	auto temp = fontAtlas->calculate_glyph(tempStr, data.fontSize);
	data.offset += fontAtlas->calculate_offset(temp, data.pivot, data.fontSize);
	charRenderingData.emplace_back(temp[0]);
	string += tempStr;
}

void szg::StringRectInstance::pop_back() {
	if (string.empty()) {
		//szgWarning()
		return;
	}
	string.pop_back();
	charRenderingData = fontAtlas->calculate_glyph(string, data.fontSize);
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

#ifdef DEBUG_FEATURES_ENABLE

#include <imgui.h>
#include <imgui_stdlib.h>

void StringRectInstance::debug_gui() {
	transform.debug_gui();

	ImGui::Separator();

	material.color.debug_gui();

	ImGui::Separator();

	ImGui::DragFloat("Font Size", &data.fontSize, 0.1f, 0.0f, 100000.0f);
	if (ImGui::DragFloat2("Pivot", &data.pivot.x, 0.1f, -1000.0f, 1000.0f)) {
		data.offset = fontAtlas->calculate_offset(charRenderingData, data.pivot, data.fontSize);
	}
	if (ImGui::InputTextMultiline("String", &string)) {
		charRenderingData = fontAtlas->calculate_glyph(string, data.fontSize);
		data.offset = fontAtlas->calculate_offset(charRenderingData, data.pivot, data.fontSize);
	}
}

#endif // DEBUG_FEATURES_ENABLE
