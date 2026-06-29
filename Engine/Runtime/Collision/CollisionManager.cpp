#include "CollisionManager.h"

#include "./CollisionFunctions.h"

#include <algorithm>
#include <ranges>

#include "Engine/Loader/ColliderGroupMapLoader.h"
#include "Engine/Runtime/Scene/World/InstanceBucket.h"

using namespace szg;

void szg::CollisionManager::setup(const std::string& sceneName) {
	const auto& checkType = std::views::keys(collisionCallbackManager.callback_functions_imm());

	for (auto& key : checkType) {
		collisionLayerList.emplace(key.big_imm(), key.small_imm());
	}

	auto [sizeRes, colliderGroupMapRes] = ColliderGroupMapLoader{}.load(sceneName);
	colliderList.resize(sizeRes);
	colliderGroupMap = std::move(colliderGroupMapRes);
}

void CollisionManager::collision_entry_point() {
	collisionCallbackManager.begin_callback();

	for (const auto& key : collisionLayerList) {
		collision(key.big_imm(), key.small_imm());
	}
}

void szg::CollisionManager::register_callback(CollisionCallbackManager::CallbackMapKey key, CollisionCallbackManager::CallbackFunctions callbackFunction) {
	collisionCallbackManager.register_callback(key.big_imm(), key.small_imm(), callbackFunction);
	// レイヤーリストに追加
	collisionLayerList.emplace(key.big_imm(), key.small_imm());
}

std::optional<i32> szg::CollisionManager::try_find_group(const std::string& groupName) {
	return colliderGroupMap.try_find_group(groupName);
}

void CollisionManager::collision(const i32 lGroupId, const i32 rGroupId) {
	// Listに存在しない名前の場合判定しない
	if (!(colliderList.size() > static_cast<size_t>(lGroupId) && colliderList.size() > static_cast<size_t>(rGroupId))) {
		return;
	}
	auto& group1 = colliderList.at(lGroupId);
	auto& group2 = colliderList.at(rGroupId);

	test_colliders(group1.sphereColliders, group2.sphereColliders);
	test_colliders(group1.aabbColliders, group2.sphereColliders);
	test_colliders(group1.aabbColliders, group2.aabbColliders);
	test_colliders(group1.sphereColliders, group2.aabbColliders);
}

void CollisionManager::remove_marked_destroy() {
	auto remove_checker = []<class T>(const Reference<T> collider) -> bool { return collider->is_marked_destroy(); };

	// 中身の削除
	for (auto& colliders : colliderList) {
		std::erase_if(colliders.aabbColliders, remove_checker);
		std::erase_if(colliders.sphereColliders, remove_checker);
	}

	// callback側の削除
	collisionCallbackManager.remove_marked_destroy();
}

void szg::CollisionManager::collect_instantiated(Reference<const InstanceBucket> instanceBucket) {
	std::ranges::for_each(instanceBucket->sphereColliders, [&](const Reference<SphereCollider>& collider) {
		register_collider(collider);
	});
	std::ranges::for_each(instanceBucket->aabbColliders, [&](const Reference<AABBCollider>& collider) {
		register_collider(collider);
	});
}

template<std::derived_from<BaseCollider> LColliderType, std::derived_from<BaseCollider> RColliderType>
void CollisionManager::test_colliders(const std::list<Reference<LColliderType>>& lhs, const std::list<Reference<RColliderType>>& rhs) {
	for (const Reference<LColliderType>& colliderL : lhs) {
		if (!colliderL->is_active()) {
			continue;
		}
		for (const Reference<RColliderType>& colliderR : rhs) {
			if (!colliderR->is_active()) {
				continue;
			}
			if constexpr (std::is_same_v<LColliderType, RColliderType>) {
				if (colliderL == colliderR) {
					continue;
				}
			}
			bool result = Collision(colliderL, colliderR);
			collisionCallbackManager.callback(
				colliderL,
				colliderR,
				result
			);
		}
	}
}
