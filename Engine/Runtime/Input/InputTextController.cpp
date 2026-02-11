#include "InputTextController.h"

#include <windows.h>

#pragma comment(lib, "Imm32.lib")

#include "Engine/Application/WinApp.h"
#include "Engine/Assets/FontAtlasMSDF/FontAtlasMSDFAsset.h"
#include "Engine/Module/World/Mesh/Primitive/CharRectStruct.h"
#include "Engine/Module/World/Mesh/Primitive/StringRectInstance.h"
#include "Engine/Runtime/Input/InputTextFrame.h"
#include "Engine/Runtime/Input/InputVKeyState.h"

szg::InputTextController::InputTextController(i32 maxLength_) {
	maxLength = maxLength_;
}

szg::InputTextController::InputTextController(const std::wstring& initString) {
	set_text(initString);
}

szg::InputTextController::InputTextController(const std::wstring& initString, i32 maxLength_) {
	set_text(initString);
	maxLength = maxLength_;
}

void szg::InputTextController::initialize() {

}

void szg::InputTextController::update() {
	if (!isInputting) {
		return;
	}

	// 入力
	const std::wstring& frameInput = InputTextFrame::FrameInputTextImm();
	for (auto& c : frameInput) {
		if (maxLength != 0 && text.size() == maxLength) {
			break;
		}
		text.insert(text.begin() + cursorPos, c);
		++cursorPos;
	}
	process_control_key();
}

void szg::InputTextController::start_input(const Vector2& screenPosition) {
	HIMC context = ImmGetContext(WinApp::GetWndHandle());
	COMPOSITIONFORM form{};
	form.ptCurrentPos.x = static_cast<LONG>(screenPosition.x);
	form.ptCurrentPos.y = static_cast<LONG>(screenPosition.y);
	ImmSetCompositionWindow(context, &form);

	ImmReleaseContext(WinApp::GetWndHandle(), context);

	isInputting = true;
}

void szg::InputTextController::end_input() {
	isInputting = false;
}

void szg::InputTextController::set_text(const std::wstring& newText) {
	text = newText;
}

const std::wstring& szg::InputTextController::text_imm() const noexcept {
	return text;
}

void szg::InputTextController::set_max_length(i32 newMaxLength) noexcept {
	maxLength = newMaxLength;
}

void szg::InputTextController::clear() noexcept {
	text.clear();
	cursorPos = 0;
}

r32 szg::InputTextController::calculate_cursor_offset(Reference<szg::StringRectInstance> stringRect) const {
	if (!stringRect) {
		return 0.0f;
	}
	auto fontAtlasAsset = stringRect->font_atlas_asset();
	if (!fontAtlasAsset) {
		return 0.0f;
	}

	std::string textA = ConvertString(text.substr(0, cursorPos));
	std::vector<szg::GlyphRenderingData> glyph = fontAtlasAsset->calculate_glyph(textA, stringRect->font_size());
	return fontAtlasAsset->calculate_advance(glyph) * stringRect->font_scale();
}

bool szg::InputTextController::is_return() const noexcept {
	return InputVKeyState::IsDownVKey(szg::VirtualKeyID::Enter) && !szg::InputTextFrame::IsImmEnter();
}

bool szg::InputTextController::is_inputting() const noexcept {
	return isInputting;
}

void szg::InputTextController::process_control_key() {
	// 削除
	if (cursorPos >= 1 && InputVKeyState::IsDownVKey(szg::VirtualKeyID::Backspace)) {
		text.erase(text.begin() + cursorPos - 1);
		--cursorPos;
	}
	if (text.size() > cursorPos && InputVKeyState::IsDownVKey(szg::VirtualKeyID::Delete)) {
		text.erase(text.begin() + cursorPos);
	}
	// 矢印
	if (InputVKeyState::IsDownVKey(szg::VirtualKeyID::LeftArrow)) {
		--cursorPos;
		if (cursorPos < 0) {
			cursorPos = 0;
		}
	}
	if (InputVKeyState::IsDownVKey(szg::VirtualKeyID::RightArrow)) {
		++cursorPos;
		if (cursorPos >= text.size()) {
			cursorPos = static_cast<i32>(text.size());
		}
	}
	// Home/End
	if (InputVKeyState::IsDownVKey(szg::VirtualKeyID::Home)) {
		cursorPos = 0;
	}
	if (InputVKeyState::IsDownVKey(szg::VirtualKeyID::End)) {
		cursorPos = static_cast<i32>(text.size());
	}
}
