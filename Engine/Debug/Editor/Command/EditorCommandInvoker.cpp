#ifdef DEBUG_FEATURES_ENABLE

#include "EditorCommandInvoker.h"

using namespace szg;

#include "IEditorCommand.h"
#include "Engine/Application/Logger.h"

void EditorCommandInvoker::Execute(std::unique_ptr<IEditorCommand> command) {
	auto& instance = GetInstance();
	if (instance.recent.has_value()) {
		// undoした部分の履歴を削除
		while (instance.recent.value() + 1 < instance.history.size()) {
			instance.history.pop_back();
		}
	}
	else {
		instance.history.clear();
	}
	// 追加
	auto& emplacedCommand = instance.history.emplace_back(std::move(command));
	// Index調整
	if (instance.recent.has_value()) {
		++instance.recent.value();
	}
	else {
		instance.recent = 0;
	}
	emplacedCommand->execute();
}

void EditorCommandInvoker::Redo() {
	auto& instance = GetInstance();
	if (instance.history.empty() || (instance.recent.has_value() && instance.recent.value() + 1 == instance.history.size())) {
		return; // 履歴がない or 先頭
	}
	if (instance.recent.has_value()) {
		++instance.recent.value();
	}
	else {
		instance.recent = 0;
	}
	instance.history[instance.recent.value()]->execute();
}

void EditorCommandInvoker::Undo() {
	auto& instance = GetInstance();
	if (!instance.recent.has_value()) {
		return;
	}
	u64 value = instance.recent.value();
	if (instance.recent.value() == 0) {
		instance.recent = std::nullopt;
	}
	else {
		--instance.recent.value();
	}
	instance.history[value]->undo();
}

void EditorCommandInvoker::ClearHistoryForce() {
	auto& instance = GetInstance();
	instance.history.clear();
	instance.recent = std::nullopt;
	szgInformation("Editor's command history is deleted.");
}

#endif // DEBUG_FEATURES_ENABLE
