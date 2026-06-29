#pragma once

#include <list>
#include <string>
#include <unordered_set>
#include <vector>

#include <Library/Utility/Template/Reference.h>

#include "./ColliderGroupMap.h"
#include "./CollisionCallbackManager.h"
#include "Engine/Module/World/Collider/BaseCollider.h"

namespace szg {

class SphereCollider;
class AABBCollider;

class InstanceBucket;

class CollisionManager final {
private:
	struct Colliders {
		std::list<Reference<SphereCollider>> sphereColliders;
		std::list<Reference<AABBCollider>> aabbColliders;
	};

public:
	CollisionManager() = default;
	~CollisionManager() = default;

	SZG_CLASS_MOVE_ONLY(CollisionManager)

public: // エンジン用
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="sceneName"></param>
	void setup(const std::string& sceneName);

	/// <summary>
	/// 衝突判定のエントリーポイント
	/// </summary>
	void collision_entry_point();

	/// <summary>
	/// シーンから削除予定のColliderを除外
	/// </summary>
	void remove_marked_destroy();

	/// <summary>
	/// シーンに生成されたColliderを収集
	/// </summary>
	/// <param name="instanceBucket"></param>
	void collect_instantiated(Reference<const InstanceBucket> instanceBucket);

public: 
	/// <summary>
	/// Callback関数を登録
	/// </summary>
	/// <param name="key"></param>
	/// <param name="callbackFunction"></param>
	void register_callback(CollisionCallbackManager::CallbackMapKey key, CollisionCallbackManager::CallbackFunctions callbackFunction);

	/// <summary>
	/// グループ名からグループIDを取得
	/// </summary>
	/// <param name="groupName"></param>
	/// <returns>見つからない場合はstd::nullopt</returns>
	std::optional<i32> try_find_group(const std::string& groupName);

private:
	/// <summary>
	/// Colliderを登録
	/// </summary>
	/// <typeparam name="ColliderType"></typeparam>
	/// <param name="collider"></param>
	template<class ColliderType>
		requires std::derived_from<ColliderType, BaseCollider>
	void register_collider(Reference<ColliderType> collider);

	void collision(const i32 lGroupId, const i32 rGroupId);

private:
	template<std::derived_from<BaseCollider> LColliderType, std::derived_from<BaseCollider> RColliderType>
	void test_colliders(
		const std::list<Reference<LColliderType>>& lhs,
		const std::list<Reference<RColliderType>>& rhs);

private:
	std::vector<Colliders> colliderList;

	std::unordered_set<SortedPair<i32>> collisionLayerList;

	ColliderGroupMap colliderGroupMap;

	CollisionCallbackManager collisionCallbackManager;
};

template<class ColliderType>
	requires std::derived_from<ColliderType, BaseCollider>
inline void CollisionManager::register_collider(Reference<ColliderType> collider) {
	Colliders& colliders = colliderList[collider->group_imm()];
	if constexpr (std::is_same_v<ColliderType, SphereCollider>) {
		colliders.sphereColliders.emplace_back(collider);
	}
	else if constexpr (std::is_same_v<ColliderType, AABBCollider>) {
		colliders.aabbColliders.emplace_back(collider);
	}
}

}; // szg
