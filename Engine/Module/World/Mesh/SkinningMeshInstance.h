#pragma once

#include "./IMultiMeshInstance.h"

#include <memory>

#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class NodeAnimationPlayer;
class SkeletonAsset;

#ifdef DEBUG_FEATURES_ENABLE
class StaticMeshInstance;
#endif // DEBUG_FEATURES_ENABLE

class SkinningMeshInstance : public IMultiMeshInstance {
public:
	struct SkeletonSpaceInstance {
		Transform3D transform;
		Affine affine;
	};

public:
	SkinningMeshInstance() noexcept;

	/// <summary>
	/// 引数付きコンストラクタ
	/// </summary>
	/// <param name="meshName">メッシュ名</param>
	/// <param name="animationName">アニメーション名</param>
	/// <param name="isLoop">ループするかどうか</param>
	explicit SkinningMeshInstance(const std::string& meshName, const std::string& animationName = "", bool isLoop = false);
	virtual ~SkinningMeshInstance() noexcept;

	SZG_CLASS_MOVE_ONLY(SkinningMeshInstance)

public:
	virtual void update() override;

	void update_animation();

public:
	void reset_animated_mesh(const std::string& meshName, const std::string& animationName = "", bool isLoop = false);
	void reset_animation(const std::string& fileName, const std::string& animationName = "", bool isLoop = false);
	NodeAnimationPlayer* const get_animation();
	const std::vector<SkeletonSpaceInstance>& joints() const { return jointInstances; }
	bool is_draw() const override;

	/// <summary>
	/// Texture、Materialパラメータ、UVデータのリセットを行う
	/// </summary>
	void default_material();

private:
	void create_skeleton();

protected:
	std::unique_ptr<NodeAnimationPlayer> nodeAnimation; // NodeAnimation

private:
	std::vector<SkeletonSpaceInstance> jointInstances; // JointのTransformとMatrix
	std::shared_ptr<const SkeletonAsset> skeletonAsset; // Skeleton関連
};

}; // szg
