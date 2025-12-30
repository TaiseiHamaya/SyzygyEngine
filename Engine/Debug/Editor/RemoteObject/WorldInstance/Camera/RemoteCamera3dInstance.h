#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IRemoteInstance.h"

#include "Engine/Module/World/Camera/Camera3D.h"

namespace szg {

class RemoteCamera3dInstance final : public IRemoteInstance<Camera3D> {
public:
	friend class EditorSceneSerializer;

public:
	RemoteCamera3dInstance() = default;
	~RemoteCamera3dInstance() = default;

	SZG_CLASS_DEFAULT(RemoteCamera3dInstance)

public:
	void update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) override;

	void draw_inspector() override;

	nlohmann::json serialize() const override;

	constexpr InstanceType instance_type() const { return InstanceType::CameraInstance; }

private:
	EditorValueField<r32> fovY{ "FOV Y", 0.45f };
	EditorValueField<r32> aspectRatio{ "AspectRatio", 1.7777f };
	EditorValueField<r32> nearClip{ "NearClip", 0.1f };
	EditorValueField<r32> farClip{ "FarClip", 1000.0f };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
