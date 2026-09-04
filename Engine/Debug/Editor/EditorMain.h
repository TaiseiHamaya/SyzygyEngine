#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <Library/Utility/Template/SingletonInterface.h>

#include "./Core/EditorGizmo.h"
#include "./Core/EditorSceneList.h"
#include "./Core/EditorSelectObject.h"
#include "./Core/CustomEditor/CustomEditorManager.h"
#include "./Core/Runtime/EditorRuntimeController.h"
#include "./Window/Hierarchy/EditorHierarchy.h"
#include "./Window/Inspector/EditorInspector.h"
#include "./Window/RenderDAG/EditorRenderDAG.h"
#include "./Window/SceneView/EditorSceneView.h"
#include "./Window/ScreenResult/EditorScreenResult.h"
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

	static void SetCustomEditorManager(std::unique_ptr<CustomEditorManager> manager);

	static bool IsHoverEditorWindow();

	static bool IsEndApplicationForce();

	static bool SeveScene();

	static void SetHotReload();

	static bool IsRuntimeInput();

	static void HandleDropFile(const std::filesystem::path& filePath);

private:
	void set_imgui_command();

	void draw_menu_bar(r32& menuHeight);

	void draw_window_buttons(r32 menuHeight);

	void draw_dock_space(r32 menuHeight);

	void execute_shortcut();

private:
	bool isActiveEditor{ true };

	bool isClosedEditor{ false };
	bool isEndApplicationForce{ false };

	bool isHotReload{ false };

	std::optional<std::string> switchSceneName;

	EditorGizmo gizmo;
	EditorSelectObject selectObject;
	EditorDeletedObjectPool deletedPool;
	EditorSceneList sceneList;
	EditorRuntimeController runtimeController;

	EditorSceneView sceneView;
	EditorScreenResult screenResult;
	EditorHierarchy hierarchy;
	EditorInspector inspector;
	EditorRenderDAG renderDAG;
	EditorAssetBrowser assetBrowser;

	std::unique_ptr<CustomEditorManager> customEditorManager;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
