#include "SkinningMeshInstance.h"

using namespace szg;

#include <Library/Utility/Tools/SmartPointer.h>

#include "Engine/Application/Logger.h"
#include "Engine/Assets/Animation/NodeAnimation/NodeAnimationPlayer.h"
#include "Engine/Assets/Animation/Skeleton/SkeletonAsset.h"
#include "Engine/Assets/Animation/Skeleton/SkeletonLibrary.h"
#include "Engine/Assets/PolygonMesh/PolygonMesh.h"
#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"
#include "Engine/Assets/Texture/TextureLibrary.h"

SkinningMeshInstance::SkinningMeshInstance() noexcept :
	IMultiMeshInstance() {
	nodeAnimation = eps::CreateUnique<NodeAnimationPlayer>();
}

SkinningMeshInstance::SkinningMeshInstance(const std::string& keyID, const std::string& animationName, bool isLoop) :
	SkinningMeshInstance() {
	reset_animated_mesh(keyID, animationName, isLoop);
}

SkinningMeshInstance::~SkinningMeshInstance() noexcept = default;

void SkinningMeshInstance::update() {
	IMultiMeshInstance::update();
	update_animation();
}

void SkinningMeshInstance::update_animation() {
	if (!isActive || !skeletonAsset) {
		return;
	}
	nodeAnimation->update();

	// Skeletonの行列計算
	const Skeleton& skeleton = skeletonAsset->skeleton();
	for (u32 jointIndex = 0; jointIndex < jointInstances.size(); ++jointIndex) {
		SkeletonSpaceInstance& jointInstance = jointInstances[jointIndex];
		const Joint& joint = skeleton.joints[jointIndex];

		// Transform更新
		// HOTFIX : Animationが存在しないかつデフォルトのTransformが変更されている場合に正しくない
		// Mixamoとかこれの典型例なので注意
		jointInstance.transform.set_scale(nodeAnimation->calculate_scale(joint.name));
		jointInstance.transform.set_quaternion(nodeAnimation->calculate_rotate(joint.name));
		jointInstance.transform.set_translate(nodeAnimation->calculate_translate(joint.name));
		// 行列更新
		if (joint.parent) {
			jointInstance.affine = Affine::FromTransform3D(jointInstance.transform) * jointInstances[joint.parent.value()].affine;
		}
		else {
			jointInstance.affine = Affine::FromTransform3D(jointInstance.transform);
		}
	}
}

void SkinningMeshInstance::reset_animated_mesh(const std::string& meshName_, const std::string& animationName, bool isLoop) {
	// メッシュ情報の取得
	if (PolygonMeshLibrary::IsRegistered(meshName_)) {
		keyID = meshName_;
	}
	else {
		keyID = "ErrorObject.obj";
	}

	default_material();
	skeletonAsset = SkeletonLibrary::GetSkeleton(keyID);
	reset_animation(keyID, animationName, isLoop);
	create_skeleton();
}

void SkinningMeshInstance::reset_animation(const std::string& fileName, const std::string& animationName, bool isLoop) {
	nodeAnimation->reset_animation(fileName + '-' + animationName);
	nodeAnimation->set_loop(isLoop);
}

NodeAnimationPlayer* const SkinningMeshInstance::get_animation() {
	return nodeAnimation.get();
}

bool SkinningMeshInstance::is_draw() const {
	return IMultiMeshInstance::is_draw() && skeletonAsset;
}

void SkinningMeshInstance::default_material() {
	std::shared_ptr<const PolygonMesh> mesh = PolygonMeshLibrary::GetPolygonMesh(keyID);

	materials.resize(mesh->material_count());

	for (i32 i = 0; auto & meshMaterial : materials) {
		// 色情報のリセット
		const auto* meshMaterialData = mesh->material_data(i);
		if (meshMaterialData) {
			// テクスチャ情報の取得
			meshMaterial.texture = TextureLibrary::GetTexture(meshMaterialData->textureFileName);
			// uv情報のリセット
			meshMaterial.uvTransform.copy(meshMaterialData->defaultUV);
		}
		else {
			meshMaterial.texture = TextureLibrary::GetTexture("Error.png");
			meshMaterial.uvTransform.copy(Transform2D{});
			szgWarning("Material data is not found.");
		}
		++i;
	}
}

void SkinningMeshInstance::create_skeleton() {
	// Skeletonが取得できない場合何もしない
	if (!skeletonAsset) {
		return;
	}

	u32 jointSize = static_cast<u32>(skeletonAsset->joint_size());

	// Jointがのサイズが0の場合おかしいので強制終了
	if (jointSize == 0) {
		skeletonAsset = nullptr;
		return;
	}

	// Jointの配列を作成
	jointInstances.resize(jointSize);
}
