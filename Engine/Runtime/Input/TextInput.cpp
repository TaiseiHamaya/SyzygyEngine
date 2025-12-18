#include "TextInput.h"

using namespace szg;

#include <Windows.h>

#include "Engine/Debug/Editor/EditorMain.h"

void szg::TextInput::BeginFrame() noexcept {
	auto& instance = GetInstance();
	instance.frameInputBuffer.clear();
	instance.isDeleteInput = false;
}

void TextInput::ProssesInput(wchar_t c, u32) {
#ifdef DEBUG_FEATURES_ENABLE
	if (!EditorMain::IsRuntimeInput()) {
		return;
	}
#endif // DEBUG_FEATURES_ENABLE
	auto& instance = GetInstance();
	switch (c) {
	case VK_BACK:
		instance.isDeleteInput = true;
		break;
	default:
		instance.frameInputBuffer.push_back(c);
		break;
	}
}

const std::wstring& szg::TextInput::FrameInputImm() noexcept {
	auto& instance = GetInstance();
	return instance.frameInputBuffer;
}

bool szg::TextInput::IsDeleteInput() noexcept {
	auto& instance = GetInstance();
	return instance.isDeleteInput;
}
