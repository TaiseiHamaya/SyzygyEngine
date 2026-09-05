#include "SceneScriptManager.h"

#include "Engine/Application/Logger.h"
#include "ISceneScript.h"

using namespace szg;

SceneScriptManager::SceneScriptManager() = default;
SceneScriptManager::~SceneScriptManager() = default;

void SceneScriptManager::register_script(std::unique_ptr<ISceneScript> script) {
	scripts.emplace_back(std::move(script));
}

void SceneScriptManager::prev_update() {
	for (auto& script : scripts) {
		if (script && !script->is_paused()) {
			script->prev_update();
		}
	}
}

void SceneScriptManager::post_update() {
	for (auto& script : scripts) {
		if (script && !script->is_paused()) {
			script->post_update();
		}
	}
}

void SceneScriptManager::finalize() {
	for (auto& script : scripts) {
		if (script) {
			script->finalize();
		}
	}
}

i64 szg::SceneScriptManager::size() const noexcept {
	return static_cast<i64>(scripts.size());
}

Reference<ISceneScript> szg::SceneScriptManager::script_mut(i64 index) noexcept {
	if (index >= size()) {
		szgWarning("Try to reference script out of range index-\'{}\'.", index);
		return nullptr;
	}
	return Reference<ISceneScript>(scripts[index].get());
}
