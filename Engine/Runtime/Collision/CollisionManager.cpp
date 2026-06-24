#include "CollisionManager.h"

#include "./CollisionFunctions.h"

#include <algorithm>
#include <ranges>

#include "Engine/Runtime/Scene/World/InstanceBucket.h"

using namespace szg;

void CollisionManager::collision_entry_point() {
	collisionCallbackManager->begin_callback();

	for (const auto& key : collisionLayerList) {
		collision(key.big_imm(), key.small_imm());
	}
}

void CollisionManager::collision(const std::string& groupName1, const std::string& groupName2) {
	// Listに存在しない名前の場合判定しない
	if (!(colliderList.contains(groupName1) && colliderList.contains(groupName2))) {
		return;
	}
	auto& group1 = colliderList.at(groupName1);
	auto& group2 = colliderList.at(groupName2);

	test_colliders(group1.sphereColliders, group2.sphereColliders);
	test_colliders(group1.aabbColliders, group2.sphereColliders);
	test_colliders(group1.aabbColliders, group2.aabbColliders);
	test_colliders(group1.sphereColliders, group2.aabbColliders);
}

void CollisionManager::remove_marked_destroy() {
	auto checker = []<class T>(const Reference<T> collider) -> bool { return collider->is_marked_destroy(); };

	// 中身の削除
	for (auto& [_, colliders] : colliderList) {
		std::erase_if(colliders.aabbColliders, checker);
		std::erase_if(colliders.sphereColliders, checker);
	}

	// 要素0のレイヤーリストを削除
	std::erase_if(colliderList, [&](const std::pair<std::string, Colliders>& list) {
		auto& value = list.second;

		if (value.aabbColliders.empty() && value.sphereColliders.empty()) {
			return true;
		}
		return false;
	});

	// callback側の削除
	collisionCallbackManager->remove_marked_destroy();
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
			collisionCallbackManager->callback(
				colliderL,
				colliderR,
				result
			);
		}
	}
}

void szg::CollisionManager::initialize_callback_body() {
	const auto& checkType = std::views::keys(collisionCallbackManager->callback_functions_imm());

	for (auto& key : checkType) {
		collisionLayerList.emplace(key.big_imm(), key.small_imm());
	}
}
