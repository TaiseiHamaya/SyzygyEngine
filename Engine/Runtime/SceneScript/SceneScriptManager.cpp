#include "SceneScriptManager.h"

#include "ISceneScript.h"

using namespace szg;

SceneScriptManager::SceneScriptManager() = default;
SceneScriptManager::~SceneScriptManager() = default;

void SceneScriptManager::register_script(std::unique_ptr<ISceneScript> script) {
	scripts.emplace_back(std::move(script));
}

void SceneScriptManager::prev_update() {
	for (auto& script : scripts) {
		if (script) {
			script->prev_update();
		}
	}
}

void SceneScriptManager::post_update() {
	for (auto& script : scripts) {
		if (script) {
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
