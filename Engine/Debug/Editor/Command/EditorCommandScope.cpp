#ifdef DEBUG_FEATURES_ENABLE

#include "EditorCommandScope.h"

#include "EditorCommandInvoker.h"

static i32 indent{ 0 };

using namespace szg;

EditorCommandScopeBegin::EditorCommandScopeBegin() : level(indent) {
}

void EditorCommandScopeBegin::execute() {
	++indent;
	if (isFirst) {
		isFirst = false;
		return;
	}
	while (indent != level) {
		EditorCommandInvoker::Redo();
	}
}

void EditorCommandScopeBegin::undo() {
	--indent;
}

EditorCommandScopeEnd::EditorCommandScopeEnd() : level(indent - 1) {
}

void EditorCommandScopeEnd::execute() {
	--indent;
}

void EditorCommandScopeEnd::undo() {
	++indent;
	while (indent != level) {
		EditorCommandInvoker::Undo();
	}
}

#endif // DEBUG_FEATURES_ENABLE
