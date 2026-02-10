#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <memory>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Template/SingletonInterface.h>

#include "../RemoteObject/IRemoteObject.h"

namespace szg {

class EditorCommandReparent;

class EditorDandDManager : public SingletonInterface<EditorDandDManager> {
	SZG_CLASS_SINGLETON(EditorDandDManager)

public:
	struct DragDataHierarchy {
		Reference<IRemoteObject> parent;
		Reference<IRemoteObject> dragging;
	};

	struct DragDataAsset {
		std::string filePath;
	};

public:
	static void CheckDandDHierarchy(Reference<IRemoteObject> self, Reference<IRemoteObject> parent);
	static void BeginDrag(Reference<IRemoteObject> self, Reference<IRemoteObject> parent);
	static void EndDrag(Reference<IRemoteObject> target);

	static void ExecuteReparent();

private:
	DragDataHierarchy dragData{};
	std::unique_ptr<EditorCommandReparent> command{ nullptr };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
