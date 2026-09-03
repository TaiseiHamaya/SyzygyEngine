#ifdef DEBUG_FEATURES_ENABLE

#include "EditorRuntimeController.h"

#include <imgui.h>

#include "Engine/Runtime/Scene/SceneManager2.h"
#include "Engine/Runtime/SceneScript/ISceneScript.h"

void szg::EditorRuntimeController::setup(const std::string& lastSelectedSceneName) {
	registeredSceneList.setup(lastSelectedSceneName);
}

void szg::EditorRuntimeController::update() {
	if (currentState == State::Pause && isStepFrameState.test(0) == true) {
		restore_world_states();
	}
	else if (currentState == State::Pause && isStepFrameState == 0b10) {
		pause_runtime();
	}

	isStepFrameState <<= 1;

	if (SceneManager2::IsEndProgram()) {
		stop_runtime();
	}
}

void szg::EditorRuntimeController::control_gui(r32 menuHight) {
	ImVec4 defualtButtonColor = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
	ImGui::SetCursorPos(ImVec2((ImGui::GetIO().DisplaySize.x - menuHight * 3) / 2, 0));
	ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, .0f);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
	// stop button
	if (ImGui::Button("\ue047")) {
		stop_runtime();
	}
	ImGui::SameLine();

	switch (currentState) {
	case szg::EditorRuntimeController::State::Play:
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		// pause button
		if (ImGui::Button("\ue034")) {
			pause_runtime();
		}
		break;
	case szg::EditorRuntimeController::State::Pause:
	case szg::EditorRuntimeController::State::Stop:
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.8f, 0.1f, 1.0f));
		// start button
		if (ImGui::Button("\ue037")) {
			start_runtime();
		}
		break;
	default:
		break;
	}
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, currentState == State::Pause ? ImVec4(0.1f, 0.1f, 0.1f, 1.0f) : defualtButtonColor);
	// step over button
	ImGui::Button("\uf6ff");
	// repeat
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left, true)) {
		isStepFrameState.set(0, true);
		ImGui::SetWindowFocus("Screen");
	}
	ImGui::SameLine();

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();

	ImGui::PushFont(nullptr, menuHight * 0.5f);
	ImGui::SetCursorPosY((menuHight - ImGui::GetFontSize()) / 2);
	ImGui::Text("   %s", currentState == State::Play ? "Play" : currentState == State::Pause ? "Pause" : "Stop");
	ImGui::PopFont();

	ImGui::SameLine();
	registeredSceneList.combo_gui();

	update();
}

void szg::EditorRuntimeController::start_runtime() {
	if (currentState == State::Play) {
		return;
	}

	if (currentState == State::Pause) {
		restore_world_states();
	}
	else {
		// 開始
		auto selectedScene = registeredSceneList.runtime_initial_scene_index();
		if (!selectedScene) {
			szgWarning("Failed to initialize the selected scene.");
			return;
		}

		worldStates.clear();
		SceneManager2::SetupInitialScene(selectedScene.value());
		SceneManager2::Setup();
	}

	currentState = State::Play;

	// window focusをゲームにする
	ImGui::SetWindowFocus("Screen");
}

void szg::EditorRuntimeController::stop_runtime() {
	if (currentState == State::Stop) {
		return;
	}

	currentState = State::Stop;

	worldStates.clear();
	SceneManager2::Finalize();
	SceneManager2::Initialize();
}

void szg::EditorRuntimeController::pause_runtime() {
	if (currentState == State::Stop) {
		return;
	}

	currentState = State::Pause;

	// 現在の状態を保存
	save_world_states();

	// 更新を停止
	Reference<Scene> scene = SceneManager2::GetCurrentScene();
	if (scene.is_null()) {
		return;
	}

	// world
	i64 worldSize = scene->world_size();
	for (i64 i : std::views::iota(0, worldSize)) {
		Reference<WorldCluster> world = scene->world_mut(i);
		if (world.is_null()) {
			break;
		}

		world->state_mut() |= WorldState::PauseUpdate;
	}

	// script
	i64 scriptSize = scene->script_size();
	for (i64 i : std::views::iota(0, scriptSize)) {
		Reference<ISceneScript> script = scene->script_mut(i);
		if (script.is_null()) {
			break;
		}

		script->set_pause(true);
	}
}

void szg::EditorRuntimeController::save_world_states() {
	Reference<Scene> scene = SceneManager2::GetCurrentScene();
	if (scene.is_null()) {
		return;
	}

	// world
	i64 worldSize = scene->world_size();
	for (i64 i : std::views::iota(0, worldSize)) {
		Reference<WorldCluster> world = scene->world_mut(i);
		if (world.is_null()) {
			break;
		}

		if (static_cast<i64>(worldStates.size()) <= i) {
			worldStates.emplace_back(world->state_imm());
		}
		worldStates[i] = world->state_imm();
	}

	// script
	i64 scriptSize = scene->script_size();
	for (i64 i : std::views::iota(0, scriptSize)) {
		Reference<ISceneScript> script = scene->script_mut(i);
		if (script.is_null()) {
			break;
		}
		if (i >= static_cast<i64>(sceneScripts.size())) {
			sceneScripts.emplace_back(script->is_paused());
		}

		sceneScripts[i] = script->is_paused();
	}
}

void szg::EditorRuntimeController::restore_world_states() {
	// 実行状態の復元
	Reference<Scene> scene = SceneManager2::GetCurrentScene();
	if (scene.is_null()) {
		return;
	}

	ImGui::SetWindowFocus("Screen");

	// world
	i64 worldSize = scene->world_size();
	for (i64 i : std::views::iota(0, worldSize)) {
		Reference<WorldCluster> world = scene->world_mut(i);
		if (world.is_null()) {
			break;
		}

		if (i >= static_cast<i64>(worldStates.size())) {
			break;
		}
		world->state_mut() = worldStates[i];
	}

	// script
	i64 scriptSize = scene->script_size();
	for (i64 i : std::views::iota(0, scriptSize)) {
		Reference<ISceneScript> script = scene->script_mut(i);
		if (script.is_null()) {
			break;
		}
		if (i >= static_cast<i64>(sceneScripts.size())) {
			break;
		}

		script->set_pause(sceneScripts[i]);
	}
}

std::optional<std::string_view> szg::EditorRuntimeController::runtime_initial_scene() const {
	return registeredSceneList.runtime_initial_scene();
}

#endif // DEBUG_FEATURES_ENABLE
