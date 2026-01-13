#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IRemoteInstance.h"

#include <string>

#include <Library/Math/ColorRGBA.h>
#include <Library/Math/Transform2D.h>

#include "Engine/Module/World/Mesh/Primitive/Rect3d.h"

namespace szg {

class RemoteRect3dInstance final : public IRemoteInstance<Rect3d, Rect3d> {
public:
	friend class EditorSceneSerializer;

	struct Material {
		std::string texture{ "Error.png" };
		EditorValueField<ColorRGBA> color{ "Color", CColorRGBA::WHITE };
		EditorValueField<Transform2D> uvTransform{ "UV Transform" };
		LighingType lightingType{ LighingType::None };
		EditorValueField<r32> shininess{ "Shininess", 50 };
	};

public:
	RemoteRect3dInstance() = default;
	~RemoteRect3dInstance() = default;

	SZG_CLASS_DEFAULT(RemoteRect3dInstance)

public:
	void setup() override;

	void update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) override;

	void draw_inspector() override;

	nlohmann::json serialize() const override;

	void on_spawn() override;

	void on_destroy() override;

	constexpr InstanceType instance_type() const { return InstanceType::Rect3dInstance; }

private:
	void reset_material();

private:
	EditorValueField<bool> isDraw{ "IsDraw", true };
	EditorValueField<u32> layer{ "Layer", 0 };

	EditorValueField<Vector2> size{ "Size", CVector2::ONE };
	EditorValueField<Vector2> pivot{ "Pivot", CVector2::ZERO };
	EditorValueField<bool> isFlipY{ "IsFlipY", false };

	Material material;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
