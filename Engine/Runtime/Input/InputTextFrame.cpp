#include "InputTextFrame.h"

using namespace szg;

#include "Engine/Debug/Editor/EditorMain.h"

void szg::InputTextFrame::BeginFrame() noexcept {
	auto& instance = GetInstance();
	instance.frameInputBuffer.clear();
	instance.isImmEnter = false;
}

void InputTextFrame::ProssesInputChar(wchar_t c, u32) {
#ifdef DEBUG_FEATURES_ENABLE
	if (!EditorMain::IsRuntimeInput()) {
		return;
	}
#endif // DEBUG_FEATURES_ENABLE
	// 修飾キー等の非入力キーは無視
	if (c <= 0x001F || (c >= 0x0080 && c <= 0x009F)) {
		return;
	}

	auto& instance = GetInstance();
	instance.frameInputBuffer.push_back(c);
}

void szg::InputTextFrame::SetImmEnter() {
#ifdef DEBUG_FEATURES_ENABLE
	if (!EditorMain::IsRuntimeInput()) {
		return;
	}
#endif // DEBUG_FEATURES_ENABLE
	auto& instance = GetInstance();
	instance.isImmEnter = true;
}

const std::wstring& szg::InputTextFrame::FrameInputTextImm() noexcept {
	auto& instance = GetInstance();
	return instance.frameInputBuffer;
}

bool szg::InputTextFrame::IsImmEnter() {
	return GetInstance().isImmEnter;
}
