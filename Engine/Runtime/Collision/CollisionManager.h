#pragma once

#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <Library/Utility/Template/Reference.h>

#include "./CollisionCallbackManager.h"
#include "Engine/Module/World/Collider/BaseCollider.h"

#ifdef DEBUG_FEATURES_ENABLE
#include "Engine/Module/DrawExecutor/PrimitiveGeometryDrawExecutor/PrimitiveGeometryDrawExecutor.h"
#endif // _DEBUG

namespace szg {

class SphereCollider;
class AABBCollider;

class CollisionManager {
private:
	struct Colliders {
		std::list<Reference<SphereCollider>> sphereColliders;
		std::list<Reference<AABBCollider>> aabbColliders;
	};

public:
	CollisionManager();
	~CollisionManager() = default;

	SZG_CLASS_MOVE_ONLY(CollisionManager)

public:
	void collision_entry_point();

	void remove_marked_destroy();

	template<class ColliderType>
	void register_collider(const std::string& groupName, Reference<ColliderType> collider);

private:
	void collision(const std::string& groupName1, const std::string& groupName2);

private:
	template<std::derived_from<BaseCollider> LColliderType, std::derived_from<BaseCollider> RColliderType>
	void test_colliders(
		const std::list<Reference<LColliderType>>& lhs,
		const std::list<Reference<RColliderType>>& rhs);

public:
	void set_callback_manager(std::unique_ptr<CollisionCallbackManager> manager) { collisionCallbackManager = std::move(manager); };

#ifdef DEBUG_FEATURES_ENABLE
public:
	void debug_gui();
	void debug_draw3d();
#endif // _DEBUG

private:
	std::unordered_map<std::string, Colliders> colliderList;

	std::unordered_set<SortedPair<std::string>> collisionLayerList;

	std::unique_ptr<CollisionCallbackManager> collisionCallbackManager;

#ifdef DEBUG_FEATURES_ENABLE
	bool isShowDebugDraw = true;
	std::unordered_set<std::string> keyList;

	std::unique_ptr<PrimitiveGeometryDrawExecutor> sphereDebugDrawExecutor;
	std::unique_ptr<PrimitiveGeometryDrawExecutor> aabbDebugDrawExecutor;
#endif // _DEBUG
};

template<class ColliderType>
inline void CollisionManager::register_collider(const std::string& groupName, Reference<ColliderType> collider) {
	Colliders& colliders = colliderList[groupName];
	if constexpr (std::is_same_v<ColliderType, SphereCollider>) {
		colliders.sphereColliders.emplace_back(collider);
	}
	else if constexpr (std::is_same_v<ColliderType, AABBCollider>) {
		colliders.aabbColliders.emplace_back(collider);
	}
	collider->set_group_name(colliderList.find(groupName)->first);
#ifdef DEBUG_FEATURES_ENABLE
	keyList.insert(groupName);
#endif // _DEBUG
}

}; // szg
