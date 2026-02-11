#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IRemoteInstance.h"

namespace szg {

class AABBCollider;

class RemoteAABBColliderInstance final : public IRemoteInstance<AABBCollider, void*> {
public:
	friend class EditorSceneSerializer;

public:
	RemoteAABBColliderInstance() = default;
	~RemoteAABBColliderInstance() = default;

	SZG_CLASS_DEFAULT(RemoteAABBColliderInstance)

public:
	void update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) override;

	void draw_inspector() override;

	nlohmann::json serialize() const override;

	constexpr InstanceType instance_type() const { return InstanceType::AABBColliderInstance; }

private:
	EditorValueField<Vector3> size{ "Size", CVector3::ONE * 2 };
	EditorValueField<Vector3> offset{ "Offset", CVector3::HALF };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
