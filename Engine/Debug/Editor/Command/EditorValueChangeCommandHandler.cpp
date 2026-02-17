#ifdef DEBUG_FEATURES_ENABLE

#include "EditorValueChangeCommandHandler.h"

#include "Engine/Application/Logger.h"

using namespace szg;

void EditorValueChangeCommandHandler::Start(std::function<void(void)> endCallFunc) {
	auto& instance = GetInstance();
	if (instance.endCallFunc) {
		End();
	}

	GetInstance().endCallFunc = endCallFunc;
}

void EditorValueChangeCommandHandler::End() {
	auto& instance = GetInstance();
	if (!instance.endCallFunc) {
		szgWarning("EditorValueChangeCommandHandler::End() was called but ValueChangeCommand has not yet generated.");
		return;
	}
	instance.endCallFunc();
	instance.endCallFunc = nullptr;
}

bool EditorValueChangeCommandHandler::IsActive() {
	return bool(GetInstance().endCallFunc);
}

#endif // DEBUG_FEATURES_ENABLE
