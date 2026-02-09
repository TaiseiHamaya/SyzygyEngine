#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <Library/Utility/Template/SingletonInterface.h>

#include "./Core/EditorGizmo.h"
#include "./Core/EditorSceneList.h"
#include "./Core/EditorSelectObject.h"
#include "./Window/EditorHierarchy.h"
#include "./Window/EditorInspector.h"
#include "./Window/EditorRenderDAG.h"
#include "./Window/EditorSceneView.h"
#include "./Window/EditorScreenResult.h"
#include "./Window/AssetBrowser/EditorAssetBrowser.h"
#include "RemoteObject/EditorDeletedObjectPool.h"

namespace szg {

class EditorMain final : public SingletonInterface<EditorMain> {
	SZG_CLASS_SINGLETON(EditorMain)

public:
	static void Initialize();
	static void Finalize();
	static void Setup();

	static void DrawBase();
	static void Draw();

public:
	static void SetActiveEditor(bool isActive);

	static bool IsHoverEditorWindow();

	static bool IsEndApplicationForce();

	static bool SeveScene();

	static void SetHotReload();

	static bool IsRuntimeInput();

private:
	void set_imgui_command();

	void draw_menu_bar(r32& menuHight);

	void draw_window_buttons(r32 menuHight);

	void draw_dock_space(r32 menuHight);

	void execute_shortcut();

private:
	bool isActiveEditor{ true };

	bool isClosedEditor{ false };
	bool isEndApplicationForce{ false };

	bool isHotReload{ false };

	std::optional<std::string> switchSceneName;

	EditorSceneView sceneView;
	EditorScreenResult screenResult;
	EditorHierarchy hierarchy;
	EditorInspector inspector;
	EditorGizmo gizmo;
	EditorSelectObject selectObject;
	EditorDeletedObjectPool deletedPool;
	EditorSceneList sceneList;
	EditorRenderDAG renderDAG;
	EditorAssetBrowser assetBrowser;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
