#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IRemoteInstance.h"

#include <variant>

#include "Engine/Module/World/Camera/CameraInstance.h"

namespace szg {

class RemoteCamera3dInstance final : public IRemoteInstance<CameraInstance> {
public:
	friend class EditorSceneSerializer;

public:
	struct PerspectiveParameters {
		EditorValueField<r32> fovY{ "FOV Y", 0.45f };
		EditorValueField<r32> aspectRatio{ "AspectRatio", 1.7777f };
		EditorValueField<r32> nearClip{ "NearClip", 0.1f };
		EditorValueField<r32> farClip{ "FarClip", 1000.0f };
	};

	struct OrthroParameters {
		EditorValueField<r32> left{ "Left", -1.0f };
		EditorValueField<r32> right{ "Right", 1.0f };
		EditorValueField<r32> bottom{ "Bottom", -1.0f };
		EditorValueField<r32> top{ "Top", 1.0f };
		EditorValueField<r32> nearClip{ "NearClip", 0.0f };
		EditorValueField<r32> farClip{ "FarClip", 1000.0f };
	};

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
	std::variant<PerspectiveParameters, OrthroParameters> projectionParameters{ PerspectiveParameters{} };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
