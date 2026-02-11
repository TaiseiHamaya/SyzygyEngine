#ifdef DEBUG_FEATURES_ENABLE

#include "EditorMain.h"

using namespace szg;

#include <fstream>

#include <imgui.h>

#include "./Core/EditorDandDManager.h"
#include "./Window/EditorLogWindow.h"
#include "Command/EditorCommandInvoker.h"
#include "Command/EditorCreateObjectCommand.h"
#include "Command/EditorDeleteObjectCommand.h"
#include "Command/EditorSelectCommand.h"
#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/Application/WinApp.h"
#include "Engine/Assets/Json/JsonAsset.h"
#include "Engine/Debug/Editor/Adapter/EditorAssetSaver.h"
#include "Engine/Debug/Editor/Core/EditorAssetContentsCollector.h"
#include "Engine/Debug/Editor/Window/AssetBrowser/Optimizer/FontAtlas/FontAtlasBuilderManager.h"
#include "Engine/Runtime/Scene/SceneManager2.h"

void EditorMain::Initialize() {
	EditorMain& instance = GetInstance();
	instance.sceneView.initialize(true);
	instance.inspector.initialize();
	instance.sceneList.initialize();
	EditorLogWindow::Initialize(true);
	instance.renderDAG.initialize();
	instance.screenResult.initialize(true);

	FontAtlasBuilderManager::Initialize();
}

void EditorMain::Finalize() {
	EditorAssetContentsCollector::Finalize();

	FontAtlasBuilderManager::Finalize();

	EditorMain& instance = GetInstance();

	nlohmann::json json;
	json["LastLoadedScene"] = instance.hierarchy.current_scene_name();

	std::filesystem::path filePath = "./Game/DebugData/Editor.json";
	auto parentPath = filePath.parent_path();
	if (!parentPath.empty() && !std::filesystem::exists(parentPath)) {
		std::filesystem::create_directories(parentPath);
	}

	std::ofstream ofstream{ filePath, std::ios_base::out };
	ofstream << std::setw(1) << std::setfill('\t') << json;
	ofstream.close();

	instance.sceneList.finalize();
	instance.renderDAG.finalize();
}

void EditorMain::Setup() {
	EditorMain& instance = GetInstance();

	EditorCreateObjectCommand::Setup(instance.deletedPool);
	EditorDeleteObjectCommand::Setup(instance.deletedPool);
	EditorSelectCommand::Setup(instance.selectObject);
	IRemoteObject::Setup(instance.sceneView);
	instance.sceneView.setup(instance.gizmo, instance.hierarchy);
	instance.inspector.setup(instance.selectObject);
	instance.hierarchy.setup(instance.selectObject, instance.sceneView);

	std::filesystem::path filePath = "./Game/DebugData/Editor.json";
	std::string sceneName;
	if (std::filesystem::exists(filePath)) {
		// 直前に開いていたシーン情報がある場合はそれを開く
		JsonAsset json{ "./Game/DebugData/Editor.json" };
		sceneName = json.try_emplace<std::string>("LastLoadedScene");
	}
	else {
		// ない場合
		szgInformation("The file required to start the editor was not found.");
		// シーン一覧を検索
		auto sceneListImm = instance.sceneList.scene_list_imm();
		if (sceneListImm.empty()) {
			// シーンが一つも登録されていない場合
			szgWarning("No scenes are registered. Create a scene first.");
			sceneName = "";
		}
		else {
			// 50音順先頭のシーンが開く
			sceneName = *sceneListImm.begin();
		}
	}

	instance.hierarchy.load(sceneName);
	instance.renderDAG.load(sceneName);
}

void EditorMain::DrawBase() {
	EditorMain& instance = GetInstance();
	if (ImGui::IsKeyPressed(ImGuiKey_F6, false)) {
		instance.isActiveEditor ^= 1;
	}

	if (!instance.isActiveEditor) {
		return;
	}

	// ホットリロード
	if (instance.isHotReload) {
		// TODO: シーンの切り替えが上手く出来てない
		Reference<Scene> currentScene = SceneManager2::GetCurrentScene();
		if (currentScene) {
			currentScene->load_assets();
			currentScene->renderDAG.setup(currentScene->name(), currentScene);
		}
		instance.isHotReload = false;
	}

	// シーン切り替え
	if (instance.switchSceneName.has_value()) {
		// シーンビューを未設定に設定
		instance.sceneView.reset_force();
		// シーンのロード
		instance.hierarchy.load(instance.switchSceneName.value());
		// DAG Editorのリセット
		instance.renderDAG.load(instance.switchSceneName.value());
		// 選択オブジェクトのリセット
		instance.selectObject.set_item(nullptr);
		// コマンドのリセット
		EditorCommandInvoker::ClearHistoryForce();
		// RenderDAGのロード
		instance.isHotReload = true;
		instance.switchSceneName = std::nullopt;
	}

	EditorAssetContentsCollector::Update();

	// HierarchyとSceneViewの同期
	instance.gizmo.begin_frame(instance.sceneView.view_origin(), instance.sceneView.view_size());
	instance.sceneView.update();
	instance.hierarchy.update_preview();
	instance.sceneView.draw_scene();

	if (!instance.isActiveEditor) {
		return;
	}

	instance.set_imgui_command();

	// ショートカット
	instance.execute_shortcut();

	instance.deletedPool.solution_sequence();
}

void EditorMain::Draw() {
	EditorMain& instance = GetInstance();
	if (!instance.isActiveEditor) {
		return;
	}

	instance.screenResult.draw();
	instance.sceneView.draw();
	instance.hierarchy.draw();
	instance.inspector.draw();
	instance.renderDAG.draw();
	instance.assetBrowser.draw();
	EditorLogWindow::Draw();
	if (instance.sceneView.is_active()) {
		ImGuizmo::SetDrawlist(instance.sceneView.draw_list().ptr());
		instance.gizmo.draw_gizmo(instance.selectObject, instance.sceneView.get_current_world_view());
	}

	EditorDandDManager::ExecuteCommand();
}

void EditorMain::SetActiveEditor(bool isActive) {
	GetInstance().isActiveEditor = isActive;
}

bool EditorMain::IsHoverEditorWindow() {
	EditorMain& instance = GetInstance();
	return instance.sceneView.is_hovered_window();
}

bool EditorMain::IsEndApplicationForce() {
	EditorMain& instance = GetInstance();
	return instance.isEndApplicationForce;
}

bool EditorMain::SeveScene() {
	EditorMain& instance = GetInstance();
	std::string sceneName = instance.hierarchy.current_scene_name();

	if (sceneName.empty()) {
		szgWarning("Scene name is empty. Save canceled.");
		return false;
	}

	instance.sceneList.add_scene(sceneName);

	std::filesystem::path sceneDirectory = std::format("./Game/Core/Scene/{}/", sceneName);
	instance.hierarchy.save(sceneDirectory);

	instance.renderDAG.save(sceneDirectory);

	EditorAssetSaver saver;
	saver.setup(instance.renderDAG, instance.hierarchy.scene_imm());
	saver.save(sceneDirectory);

	szgInformation("Scene file saved. ({})", sceneName);

	return true;
}

void EditorMain::SetHotReload() {
	EditorMain& instance = GetInstance();
	instance.isHotReload = true;
}

bool szg::EditorMain::IsRuntimeInput() {
	// エディターのシーンビューがフォーカスされている
	// or
	// エディターが無効の場合のみ受け取る
	EditorMain& instance = GetInstance();
	return
		instance.screenResult.is_focus() ||
		!instance.isActiveEditor;
}

void szg::EditorMain::HandleDropFile(const std::filesystem::path& filePath) {
	EditorMain& instance = GetInstance();
	instance.assetBrowser.on_drop_file(filePath);
}

void EditorMain::set_imgui_command() {
	r32 menuHight{ 0 };

	draw_menu_bar(menuHight);

	draw_dock_space(menuHight);
}

void szg::EditorMain::draw_menu_bar(r32& menuHight) {
	// メニューバーの表示
	if (ImGui::BeginMainMenuBar()) {
		// Windowメニュー
		bool isOpen;
		menuHight = ImGui::GetWindowSize().y;
		ImGui::SetCursorPos({ 1.0f,1.0f });
		ImGui::PushFont(nullptr, menuHight * 0.5f);
		isOpen = ImGui::BeginMenu("Window");
		ImGui::PopFont();
		if (isOpen) {
			screenResult.draw_menu("ScreenView");
			sceneView.draw_menu("Scene");
			hierarchy.draw_menu("Hierarchy");
			inspector.draw_menu("Inspector");
			EditorLogWindow::DrawMenu("Log");
			renderDAG.draw_menu("RenderDAG");
			ImGui::EndMenu();
		}
		ImGui::PushFont(nullptr, menuHight * 0.5f);

		// Editメニュー
		isOpen = ImGui::BeginMenu("Edit");
		ImGui::PopFont();
		if (isOpen) {
			if (ImGui::BeginMenu("Scene")) {
				std::string currentSceneName = hierarchy.current_scene_name();
				if (sceneList.scene_list_gui(currentSceneName)) {
					switchSceneName = currentSceneName;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		draw_window_buttons(menuHight);

		ImGui::EndMainMenuBar();
	}
}

void szg::EditorMain::draw_window_buttons(r32 menuHight) {
	ImGui::PushFont(nullptr, menuHight * 0.75f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));

	// 最小化
	ImGui::SameLine();
	ImGui::SetCursorPos({ ImGui::GetWindowSize().x - menuHight * 2 , 0.0f });
	if (ImGui::Button("\ue931")) {
		ShowWindow(WinApp::GetWndHandle(), SW_MINIMIZE);
	}

	// 最大化
	ImGui::SameLine();
	ImGui::SetCursorPos({ ImGui::GetWindowSize().x - menuHight, 0.0f });
	if (ImGui::Button("\ue5cd")) {
		ImGui::OpenPopup("未保存の項目があります");
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopFont();

	// 終了確認ポップアップ
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("未保存の項目があります", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.235f, 0.471f, 0.847f, 0.5f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.067f, 0.333f, 0.8f, 0.5f });
		if (ImGui::Button("保存して終了")) {
			if (SeveScene()) {
				isEndApplicationForce = true;
			}
		}
		ImGui::PopStyleColor(2);
		ImGui::SameLine();
		if (ImGui::Button("保存しないで終了")) {
			isEndApplicationForce = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("キャンセル")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void szg::EditorMain::draw_dock_space(r32 menuHight) {
	i32 flags =
		ImGuiWindowFlags_MenuBar | // メニューバーを表示
		ImGuiWindowFlags_NoDocking | // ドッキングしない
		ImGuiWindowFlags_NoTitleBar | // タイトルバーなし
		ImGuiWindowFlags_NoNavFocus | // フォーカスしない
		ImGuiWindowFlags_NoMove | // 移動しない
		ImGuiWindowFlags_NoScrollWithMouse | // スクロールしない
		ImGuiWindowFlags_NoResize | // リサイズしない
		ImGuiWindowFlags_NoScrollbar | // スクロールバーなし
		ImGuiWindowFlags_NoBringToFrontOnFocus; // 最背面

	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ ProjectSettings::ClientSize().x, ProjectSettings::ClientSize().y });

	ImGui::Begin("EditorMain", nullptr, flags);

	// メインのドックスペースを追加
	ImGuiID dockSpaceId = ImGui::GetID("EditorMain");
	ImVec2 one = { 1.0f, 1.0f };
	ImVec2 cursorPos = ImVec2{ 0.0f, menuHight } + one;
	ImGui::SetCursorPos(cursorPos);
	ImVec2 baseSize = ImVec2{ ProjectSettings::ClientSize().x, ProjectSettings::ClientSize().y };
	ImVec2 editorSize = baseSize - cursorPos - one;
	ImGui::DockSpace(dockSpaceId, editorSize, ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::End();
}

void szg::EditorMain::execute_shortcut() {
	// Undo
	if (ImGui::Shortcut(ImGuiKey_Z | ImGuiMod_Ctrl, ImGuiInputFlags_RouteAlways | ImGuiInputFlags_Repeat)) {
		EditorCommandInvoker::Undo();
	}
	// Redo
	if (ImGui::Shortcut(ImGuiKey_Z | ImGuiMod_Ctrl | ImGuiMod_Shift, ImGuiInputFlags_RouteAlways | ImGuiInputFlags_Repeat)) {
		EditorCommandInvoker::Redo();
	}
	// 保存
	if (ImGui::Shortcut(ImGuiKey_S | ImGuiMod_Ctrl, ImGuiInputFlags_RouteAlways)) {
		SeveScene();
	}
}

#endif // DEBUG_FEATURES_ENABLE
