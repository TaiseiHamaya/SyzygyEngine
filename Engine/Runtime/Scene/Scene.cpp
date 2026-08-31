#include "Scene.h"

#include <filesystem>

#include "Engine/Loader/SceneAssetListLoader.h"

using namespace szg;

void Scene::load_assets() {
	assetCollection = SceneAssetListLoader{}.load(sceneName);
	assetCollection.load_assets();
}

void Scene::custom_load_asset() {
}

void Scene::initialize() {
	assetCollection.load_lazy_assets();
}

void Scene::setup() {
	// フォルダ内のワールドを全て読み込む
	std::filesystem::path filePath = std::format("./Game/Core/Scene/{}/Worlds", sceneName);
	if (std::filesystem::exists(filePath) == false) { // ファイルが存在しない
		szgWarning("Scene-\'{}\' world setup folder not found.", sceneName);
		return;
	}
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(filePath)) {
		std::unique_ptr<WorldCluster> world = std::make_unique<WorldCluster>();

		world->initialize();
		world->setup(entry.path());

		worlds.emplace_back(std::move(world));
	}
	szgWarningIf(worlds.empty(), "Scene-\'{}\' has no worlds.", sceneName);

	// 描画パスの初期化
	renderDAG.setup(sceneName, this);
}

void Scene::begin_frame() {
	for (std::unique_ptr<WorldCluster>& world : worlds) {
		world->begin_frame();
	}
}

void Scene::update() {
	sceneScriptManager.prev_update();
	for (std::unique_ptr<WorldCluster>& world : worlds) {
		world->update();
	}
	sceneScriptManager.post_update();
}

void Scene::pre_draw() {
	for (std::unique_ptr<WorldCluster>& world : worlds) {
		world->pre_draw();
	}

	for (std::unique_ptr<WorldCluster>& world : worlds) {
		world->post_update();
	}
}

void Scene::draw() const {
	renderDAG.render_entry_point();
}

void Scene::end_frame() {
	for (std::unique_ptr<WorldCluster>& world : worlds) {
		world->end_frame();
	}
}

void Scene::finalize() {
	sceneScriptManager.finalize();
}

i64 szg::Scene::world_size() const noexcept {
	return static_cast<i64>(worlds.size());
}

Reference<WorldCluster> Scene::world_mut(i64 index) noexcept {
	if (index >= world_size()) {
		szgWarning("Try to reference world out of range index-\'{}\'.", index);
		return nullptr;
	}
	return worlds[index];
}

i64 szg::Scene::script_size() const noexcept {
	return sceneScriptManager.size();
}

Reference<ISceneScript> szg::Scene::script_mut(i64 index) noexcept {
	return sceneScriptManager.script_mut(index); // SceneScriptManager内でnullチェック済み
}

void Scene::set_name(const std::string& name) {
	sceneName = name;
}

std::string_view Scene::name() const noexcept {
	return sceneName;
}
