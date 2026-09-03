#ifdef DEBUG_FEATURES_ENABLE

#include "RuntimeRegisteredSceneList.h"

#include <imgui.h>

#include "Engine/Runtime/Scene/SceneManager2.h"
#include "Engine/Runtime/Scene/BaseSceneFactory.h"

using namespace szg;

void RuntimeRegisteredSceneList::setup(const std::string& lastSelectedSceneName) {
	currentSelectedSceneName = lastSelectedSceneName;
	auto sceneFactory = SceneManager2::SceneFactoryImm();
	if (sceneFactory.is_null()) {
		return;
	}

	for (i32 i = 0;; ++i) {
		auto sceneTemp = sceneFactory->create_scene2(i);
		if (!sceneTemp) {
			break;
		}

		sceneNameToIndexMap.emplace(std::format("{}({})", sceneTemp->name(), i), i);
	}

	if (!sceneNameToIndexMap.contains(currentSelectedSceneName)) {
		szgWarning("Last selected scene name '{}' is not found in the registered scene list.", currentSelectedSceneName);
	}
}

void RuntimeRegisteredSceneList::combo_gui() {
	ImGui::SetNextItemWidth(250.0f);
	if (ImGui::BeginCombo("##Initialize Scene", currentSelectedSceneName.c_str())) {
		for (const auto& [name, index] : sceneNameToIndexMap) {
			bool isSelected = (currentSelectedSceneName == name);
			if (ImGui::Selectable(name.c_str(), isSelected)) {
				currentSelectedSceneName = name;
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		

		ImGui::EndCombo();
	}
}

std::optional<std::string_view> RuntimeRegisteredSceneList::runtime_initial_scene() const {
	if (!sceneNameToIndexMap.contains(currentSelectedSceneName)) {
		return std::nullopt;
	}
	return currentSelectedSceneName;
}

std::optional<i32> RuntimeRegisteredSceneList::runtime_initial_scene_index() const {
	if (!sceneNameToIndexMap.contains(currentSelectedSceneName)) {
		return std::nullopt;
	}
	return sceneNameToIndexMap.at(currentSelectedSceneName);
}

#endif // DEBUG_FEATURES_ENABLE
