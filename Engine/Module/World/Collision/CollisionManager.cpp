#include "CollisionManager.h"

#include <ranges>

#include <Library/Utility/Tools/SmartPointer.h>

#include "Engine/Module/World/Collision/CollisionFunctions.h"
#include "Engine/Assets/PrimitiveGeometry/PrimitiveGeometryLibrary.h"

using namespace szg;

CollisionManager::CollisionManager() {
#ifdef DEBUG_FEATURES_ENABLE

	sphereDebugDrawExecutor = eps::CreateUnique<PrimitiveGeometryDrawExecutor>(
		PrimitiveGeometryLibrary::GetPrimitiveGeometry("Sphere"), 1024
	);
	aabbDebugDrawExecutor = eps::CreateUnique<PrimitiveGeometryDrawExecutor>(
		PrimitiveGeometryLibrary::GetPrimitiveGeometry("Box"), 1024
	);

#endif // _DEBUG
}

void CollisionManager::collision_entry_point() {
	collisionCallbackManager->begin_callback();

#undef small
	for (const auto& key : collisionLayerList) {
		collision(key.big(), key.small());
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
#ifdef DEBUG_FEATURES_ENABLE
			keyList.erase(list.first);
#endif // _DEBUG
			return true;
		}
		return false;
	});

	// callback側の削除
	collisionCallbackManager->remove_marked_destroy();
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

#ifdef DEBUG_FEATURES_ENABLE

#include <format>
#include <imgui.h>

void CollisionManager::debug_gui() {
	ImGui::Checkbox("ShowDebugDraw", &isShowDebugDraw);
	if (ImGui::TreeNode(std::format("ColliderList##{:}", (void*)this).c_str())) {
		for (auto& name : keyList) {
			if (colliderList.contains(name)) {
				auto& list = colliderList.at(name);
				ImGui::Text(
					std::format("{} : {}", name,
						list.aabbColliders.size() + list.sphereColliders.size()).c_str()
				);
			}
		}
		ImGui::TreePop();
	}
}

void CollisionManager::debug_draw3d() {
	if (!isShowDebugDraw) {
		return;
	}

	sphereDebugDrawExecutor->begin();
	aabbDebugDrawExecutor->begin();

	for (const Colliders& colliders : colliderList | std::views::values) {
		for (const Reference<SphereCollider>& sphereCollider : colliders.sphereColliders) {
			sphereDebugDrawExecutor->write_to_buffer(sphereCollider->debug_matrix());
		}

		for (const Reference<AABBCollider>& aabbCollider : colliders.aabbColliders) {
			aabbDebugDrawExecutor->write_to_buffer(aabbCollider->debug_matrix());
		}
	}

	sphereDebugDrawExecutor->draw_command();
	aabbDebugDrawExecutor->draw_command();
}
#endif // _DEBUG
