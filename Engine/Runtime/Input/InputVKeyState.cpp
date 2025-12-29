#include "InputVKeyState.h"

using namespace szg;

#include "Engine/Debug/Editor/EditorMain.h"

void szg::InputVKeyState::BeginFrame() noexcept {
	auto& instance = GetInstance();
	instance.virtualKeys.clear();
}

void szg::InputVKeyState::OnKeyDown(VirtualKeyID key, u32) {
#ifdef DEBUG_FEATURES_ENABLE
	if (!EditorMain::IsRuntimeInput()) {
		return;
	}
#endif // DEBUG_FEATURES_ENABLE
	auto& instance = GetInstance();
	instance.virtualKeys.emplace(key);
}

bool szg::InputVKeyState::IsDownVKey(VirtualKeyID keyId) noexcept {
	return GetInstance().virtualKeys.contains(keyId);
}
