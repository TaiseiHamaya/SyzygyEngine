#ifdef DEBUG_FEATURES_ENABLE

#include "EditorSceneList.h"

using namespace szg;

#include <fstream>

#include <imgui.h>

#include "Engine/Assets/Json/JsonAsset.h"

void EditorSceneList::initialize() {
	JsonAsset json;
	json.load("./Game/Core/Game.json");
	if (json.cget().contains("SceneList")) {
		for (const auto& scene : json.cget()["SceneList"]) {
			if (scene.is_string()) {
				sceneList.emplace(scene.get<std::string>());
			}
		}
	}
}

void EditorSceneList::finalize() {
	export_scene_list();
}

bool EditorSceneList::scene_list_gui(std::string& current) {
	for (i32 i = 0; const auto& scene : sceneList) {
		std::string tag = std::format("{}##Scene{}", scene, i);
		if (ImGui::Selectable(tag.c_str(), current == scene)) {
			current = scene;
			return true;
		}
		++i;
	}
	return false;
}

void EditorSceneList::add_scene(const std::string& sceneName) {
	sceneList.emplace(sceneName);
}

const std::set<std::string>& szg::EditorSceneList::scene_list_imm() const noexcept {
	return sceneList;
}

void EditorSceneList::export_scene_list() {
	nlohmann::json json;
	for (const auto& scene : sceneList) {
		json["SceneList"].emplace_back(scene);
	}

	std::filesystem::path filePath = "./Game/Core/Game.json";
	auto parentPath = filePath.parent_path();
	if (!parentPath.empty() && !std::filesystem::exists(parentPath)) {
		std::filesystem::create_directories(parentPath);
	}

	std::ofstream ofstream{ filePath, std::ios_base::out };
	ofstream << std::setw(1) << std::setfill('\t') << json;
	ofstream.close();
}

#endif // DEBUG_FEATURES_ENABLE
