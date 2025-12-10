#include "TextInput.h"

using namespace szg;

void szg::TextInput::BeginFrame() noexcept {
	auto& instance = GetInstance();
	instance.frameInputBuffer.clear();
}

void TextInput::ProssesInput(wchar_t c, u32) {
	auto& instance = GetInstance();
	instance.frameInputBuffer.push_back(c);
}

const std::wstring& szg::TextInput::FrameInputImm() noexcept {
	auto& instance = GetInstance();
	return instance.frameInputBuffer;
}
