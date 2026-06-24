#pragma once

#include <vector>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class WorldInstance;

class CameraInstance;
class StaticMeshInstance;
class SkinningMeshInstance;
class DirectionalLightInstance;
class PointLightInstance;
class Rect3d;
class StringRectInstance;

class SphereCollider;
class AABBCollider;

class InstanceBucket final {
	friend class WorldRenderCollection;
	friend class CollisionManager;

public:
	InstanceBucket() = default;
	~InstanceBucket() = default;

	SZG_CLASS_MOVE_ONLY(InstanceBucket)

public:
	void reset();

	template<typename T>
		requires std::derived_from<T, WorldInstance>
	void register_instance(Reference<T> instance);

private:
	std::vector<Reference<CameraInstance>> camera;
	std::vector<Reference<StaticMeshInstance>> staticMesh;
	std::vector<Reference<SkinningMeshInstance>> skinMesh;
	std::vector<Reference<DirectionalLightInstance>> directionalLightInstance;
	std::vector<Reference<PointLightInstance>> pointLightInstance;
	std::vector<Reference<Rect3d>> rect;
	std::vector<Reference<StringRectInstance>> stringRect;

	std::vector<Reference<SphereCollider>> sphereColliders;
	std::vector<Reference<AABBCollider>> aabbColliders;
};

template<typename T>
	requires std::derived_from<T, WorldInstance>
void InstanceBucket::register_instance(Reference<T> instance) {
	if constexpr (std::derived_from<T, CameraInstance>) {
		camera.emplace_back(instance);
	}
	else if constexpr (std::derived_from<T, StaticMeshInstance>) {
		staticMesh.emplace_back(instance);
	}
	else if constexpr (std::derived_from<T, SkinningMeshInstance>) {
		skinMesh.emplace_back(instance);
	}
	else if constexpr (std::derived_from<T, DirectionalLightInstance>) {
		directionalLightInstance.emplace_back(instance);
	}
	else if constexpr (std::derived_from<T, PointLightInstance>) {
		pointLightInstance.emplace_back(instance);
	}
	else if constexpr (std::derived_from<T, Rect3d>) {
		rect.emplace_back(instance);
	}
	else if constexpr (std::derived_from<T, StringRectInstance>) {
		stringRect.emplace_back(instance);
	}
	else if constexpr (std::derived_from<T, SphereCollider>) {
		sphereColliders.emplace_back(instance);
	}
	else if constexpr (std::derived_from<T, AABBCollider>) {
		aabbColliders.emplace_back(instance);
	}
}

}; // szg
