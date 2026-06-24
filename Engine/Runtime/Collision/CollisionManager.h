#pragma once

#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <Library/Utility/Template/Reference.h>

#include "./CollisionCallbackManager.h"
#include "Engine/Module/World/Collider/BaseCollider.h"

namespace szg {

class SphereCollider;
class AABBCollider;

class InstanceBucket;

class CollisionManager {
private:
	struct Colliders {
		std::list<Reference<SphereCollider>> sphereColliders;
		std::list<Reference<AABBCollider>> aabbColliders;
	};

public:
	CollisionManager() = default;
	~CollisionManager() = default;

	SZG_CLASS_MOVE_ONLY(CollisionManager)

public:
	template<typename T>
		requires std::derived_from<T, CollisionCallbackManager>
	void initialize_callback();

	void collision_entry_point();

	void remove_marked_destroy();

	void collect_instantiated(Reference<const InstanceBucket> instanceBucket);

	template<class ColliderType>
		requires std::derived_from<ColliderType, BaseCollider>
	void register_collider(Reference<ColliderType> collider);

private:
	void collision(const std::string& groupName1, const std::string& groupName2);

private:
	template<std::derived_from<BaseCollider> LColliderType, std::derived_from<BaseCollider> RColliderType>
	void test_colliders(
		const std::list<Reference<LColliderType>>& lhs,
		const std::list<Reference<RColliderType>>& rhs);

	void initialize_callback_body();

private:
	std::unordered_map<std::string, Colliders> colliderList;

	std::unordered_set<SortedPair<std::string>> collisionLayerList;

	std::unique_ptr<CollisionCallbackManager> collisionCallbackManager;
};

template<typename T>
	requires std::derived_from<T, CollisionCallbackManager>
void CollisionManager::initialize_callback() {
	collisionCallbackManager = std::make_unique<T>();

	initialize_callback_body();
}

template<class ColliderType>
		requires std::derived_from<ColliderType, BaseCollider>
inline void CollisionManager::register_collider(Reference<ColliderType> collider) {
	Colliders& colliders = colliderList[collider->group()];
	if constexpr (std::is_same_v<ColliderType, SphereCollider>) {
		colliders.sphereColliders.emplace_back(collider);
	}
	else if constexpr (std::is_same_v<ColliderType, AABBCollider>) {
		colliders.aabbColliders.emplace_back(collider);
	}
	collider->set_group_name(colliderList.find(collider->group())->first);
}

}; // szg
