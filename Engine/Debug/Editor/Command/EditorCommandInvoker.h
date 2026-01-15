#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <Library/Utility/Template/SingletonInterface.h>

#include <deque>
#include <optional>
#include <memory>

namespace szg {

class IEditorCommand;

class EditorCommandInvoker final : public SingletonInterface<EditorCommandInvoker> {
	SZG_CLASS_SINGLETON(EditorCommandInvoker)

public:
	static void Execute(std::unique_ptr<IEditorCommand> command);
	static void Redo();
	static void Undo();
	static void ClearHistoryForce();

private:
	std::optional<u64> recent;
	std::deque<std::unique_ptr<IEditorCommand>> history;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
