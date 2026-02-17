#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "IEditorCommand.h"

#include "../RemoteObject/IRemoteObject.h"

#include <Library/Utility/Template/Reference.h>

namespace szg {

class EditorCommandReparent final : public IEditorCommand {
public:
	EditorCommandReparent(Reference<IRemoteObject> target, Reference<IRemoteObject> recent, Reference<IRemoteObject> next);
	~EditorCommandReparent() = default;

	SZG_CLASS_MOVE_ONLY(EditorCommandReparent)

public:
	void execute() override;
	void undo() override;

private:
	Reference<IRemoteObject> target;
	Reference<IRemoteObject> recentParent;
	Reference<IRemoteObject> nextParent;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
