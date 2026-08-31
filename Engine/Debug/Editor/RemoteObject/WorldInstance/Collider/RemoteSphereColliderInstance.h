#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IRemoteInstance.h"

namespace szg {

class SphereCollider;

class RemoteSphereColliderInstance final : public IRemoteInstance<SphereCollider, void*> {
public:
	friend class EditorSceneSerializer;

public:
	RemoteSphereColliderInstance() = default;
	~RemoteSphereColliderInstance() = default;

	SZG_CLASS_DEFAULT(RemoteSphereColliderInstance)

public:
	void update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) override;

	void draw_inspector() override;

	nlohmann::json serialize() const override;

	constexpr InstanceType instance_type() const { return InstanceType::SphereColliderInstance; }

private:
	EditorValueField<r32> radius{ "Radius", 1.0f };
	i32 colliderId{ 0 };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
