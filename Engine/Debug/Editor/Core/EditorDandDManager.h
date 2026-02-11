#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <memory>
#include <optional>
#include <string>
#include <variant>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Template/SingletonInterface.h>

#include "../RemoteObject/IRemoteObject.h"
#include "Engine/Assets/AssetTypeEnum.h"

namespace szg {

class IEditorCommand;

class EditorDandDManager : public SingletonInterface<EditorDandDManager> {
	SZG_CLASS_SINGLETON(EditorDandDManager)

public:
	struct DragDataHierarchy {
		Reference<IRemoteObject> parent;
		Reference<IRemoteObject> dragging;
	};

	struct DragDataAsset {
		AssetType assetType;
		std::string filePath;
	};

public:
	static void CheckDandDHierarchy(Reference<IRemoteObject> self, Reference<IRemoteObject> parent);

	static void BeginDragHierarchy(Reference<IRemoteObject> self, Reference<IRemoteObject> parent);
	static void EndDragHierarchy(Reference<IRemoteObject> target);

	static void BeginDragAsset(AssetType assetType, const std::string& filePath);
	static std::optional<DragDataAsset> AcceptAssetDrop();

	static void ExecuteCommand();

private:
	std::variant<std::monostate, DragDataHierarchy, DragDataAsset> currentDragData{ std::monostate{} };
	std::unique_ptr<IEditorCommand> command{ nullptr };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
