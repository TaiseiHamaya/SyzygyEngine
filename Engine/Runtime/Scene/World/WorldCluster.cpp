#include "WorldCluster.h"

#include "Engine/Assets/Json/JsonAsset.h"
#include "Engine/Loader/WorldInstanceLoader.h"

using namespace szg;

void WorldCluster::initialize() {
	worldRoot.initialize();
	worldRenderCollection.initialize();
}

void WorldCluster::setup(const std::filesystem::path& setupFile) {
	worldRoot.setup(instanceBucket);

	// 読み込み
	JsonAsset json{ setupFile };
	WorldInstanceLoader loader;
	loader.setup(worldRoot);
	if (json.cget().contains("Instances") && json.cget()["Instances"].is_array()) {
		// 各WorldInstanceの生成
		for (const nlohmann::json& instanceJson : json.cget()["Instances"]) {
			loader.entry_point(instanceJson, nullptr);
		}
	}

	// 描画レイヤー数の設定
	u8 numLayer = json.cget().value<u8>("NumLayer", 0);
	worldRenderCollection.setup(numLayer);
}

void WorldCluster::begin_frame() {
	if (state & WorldState::PauseUpdate) {
		return;
	}
	// ---------- Instantiate後の処理 ----------
	// 描画が側に伝達
	worldRenderCollection.collect_instantiated(instanceBucket);
	collisionManager.collect_instantiated(instanceBucket);
	instanceBucket.reset();
	// ---------- 削除予定インスタンス処理 ----------
	// 描画に関して
	worldRenderCollection.remove_marked_destroy();
	// コリジョン
	collisionManager.remove_marked_destroy();
	// 実際の削除
	worldRoot.delete_marked_destroy();
}

void WorldCluster::update() {
	if (state & WorldState::PauseUpdate) {
		return;
	}
	worldRoot.update();

	collisionManager.collision_entry_point();

	worldRoot.update_affine();
}

void WorldCluster::pre_draw() {
	worldRenderCollection.reset_buffer();

	if (!(state & WorldState::PauseDraw)) {
		worldRenderCollection.transfer();
	}

	if (!(state & WorldState::PauseUpdate)) {
		worldRoot.post_update();
	}
}

void WorldCluster::end_frame() {
}

Reference<WorldRoot> WorldCluster::world_root_mut() {
	return worldRoot;
}

Reference<const WorldRenderCollection> WorldCluster::render_collection_imm() {
	return worldRenderCollection;
}

Reference<CollisionManager> szg::WorldCluster::collision_manager_mut() {
	return collisionManager;
}
