#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IRemoteInstance.h"

#include <string>
#include <vector>

#include <Library/Math/Transform2D.h>

#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"
#include "Engine/Module/World/Mesh/StaticMeshInstance.h"
#include "Engine/Debug/Editor/Adapter/EditorAssetField.h"

#define COLOR_RGB_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

namespace szg {

class SkinningMeshInstance;

class SkeletonAsset;

class RemoteSkinningMeshInstance final : public IRemoteInstance<SkinningMeshInstance, StaticMeshInstance> {
public:
	friend class EditorSceneSerializer;

public:
	struct Material {
		std::string texture;
		ColorRGB color;
		Transform2D uvTransform;
		LighingType lightingType{ LighingType::HalfLambert };
		r32 shininess;
	};

public:
	RemoteSkinningMeshInstance() noexcept;
	~RemoteSkinningMeshInstance() noexcept;

public:
	void setup() override;

	void update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) override;

	void draw_inspector() override;

	nlohmann::json serialize() const override;

	constexpr InstanceType instance_type() const { return InstanceType::SkinningMeshInstance; }

	void on_spawn() override;

	void on_destroy() override;

private:
	void default_material();

private:
	EditorValueField<bool> isDraw{ "IsDraw" };
	EditorValueField<u32> layer{ "Layer" };

	EditorAssetField meshName{ "Mesh", AssetType::Mesh };
	std::vector<Material> materials;

	std::string animationName;
	EditorValueField<bool> isLoop{ "IsLoop" };

	std::shared_ptr<const SkeletonAsset> skeleton;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
