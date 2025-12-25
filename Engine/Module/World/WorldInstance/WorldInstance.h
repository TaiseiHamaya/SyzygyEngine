#pragma once

#include <Library/Math/Affine.h>
#include <Library/Math/Hierarchy.h>
#include <Library/Math/Transform3D.h>
#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class WorldRoot;

class WorldInstance {
#ifdef DEBUG_FEATURES_ENABLE
	friend class RemoteWorldInstance;
#endif // DEBUG_FEATURES_ENABLE

public:
	WorldInstance() noexcept = default;
	virtual ~WorldInstance() = default;

	SZG_CLASS_MOVE_ONLY(WorldInstance)

public:
	/// <summary>
	/// 開始処理
	/// </summary>
	virtual void begin() {};

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void update() {};

	/// <summary>
	/// update_affineの直前に呼ばれる
	/// </summary>
	virtual void fixed_update() {};

	/// <summary>
	/// Affine行列の更新
	/// </summary>
	virtual void update_affine();

	/// <summary>
	/// 全てのInstanceのAffine行列の更新後に呼ばれる
	/// </summary>
	virtual void late_update() {};

private:
	/// <summary>
	/// WorldAffineの作成
	/// </summary>
	/// <returns></returns>
	Affine create_world_affine() const;

public:
	/// <summary>
	/// Targetの方向を向く
	/// </summary>
	/// <param name="target">向く方向</param>
	/// <param name="upward">上方向</param>
	void look_at(Reference<const WorldInstance> target, r32 angle = 0.0f, const Vector3& upward = CVector3::BASIS_Y) noexcept;

	/// <summary>
	/// pointの方向を向く
	/// </summary>
	/// <param name="point">World座標系上の点</param>
	/// <param name="upward">上方向</param>
	void look_at(const Vector3& point, r32 angle = 0.0f, const Vector3& upward = CVector3::BASIS_Y) noexcept;

	/// <summary>
	/// 軸LookAt(軸ビルボード)
	/// </summary>
	/// <param name="target"></param>
	/// <param name="angle"></param>
	/// <param name="axis"></param>
	void look_at_axis(Reference<const WorldInstance> target, r32 angle = 0.0f, const Vector3& axis = CVector3::BASIS_Y) noexcept;

	/// <summary>
	/// 軸LookAt(軸ビルボード)
	/// </summary>
	/// <param name="point"></param>
	/// <param name="angle"></param>
	/// <param name="axis"></param>
	void look_at_axis(const Vector3& point, r32 angle = 0.0f, const Vector3& axis = CVector3::BASIS_Y) noexcept;

	/// <summary>
	/// 階層構造の震度
	/// </summary>
	/// <returns></returns>
	u32 depth() const { return hierarchyDepth; };

	/// <summary>
	/// Transformの取得(Const)
	/// </summary>
	/// <returns></returns>
	const Transform3D& get_transform() const { return transform; };

	/// <summary>
	/// Transformの取得(非Const)
	/// </summary>
	/// <returns></returns>
	Transform3D& get_transform() { return transform; };

	/// <summary>
	/// Hierarchyの親アドレスの取得
	/// </summary>
	/// <returns>存在しなければnullptr</returns>
	Reference<const WorldInstance> parent_imm() const noexcept;
	Reference<WorldInstance> parent_mut() noexcept;

	/// <summary>
	/// World行列の取得
	/// </summary>
	/// <returns></returns>
	const Affine& world_affine() const { return affine; };

	/// <summary>
	/// WorldPositionの取得
	/// </summary>
	/// <returns></returns>
	const Vector3& world_position() const { return affine.get_origin(); };

	/// <summary>
	/// 親を再設定
	/// </summary>
	/// <param name="parent">親のInstance</param>
	/// <param name="isKeepPose">現在の姿勢を維持するかどうか</param>
	void reparent(Reference<WorldInstance> parent, bool isKeepPose = true);

	// ----- Active関連 -----
	void set_active(bool isActive_) { isActive = isActive_; };
	bool is_active() const { return isActive; };

	// ----- Destroyフラグ -----
	void destroy_self();
	void mark_destroy();
	bool is_marked_destroy() const { return isDestroy; }
	virtual void on_mark_destroy() {};

	// ----- id関連 -----
	void setup_id(u64 id);
	u64 instance_id() const;

	// ----- WorldRoot -----
	void setup_world_root(Reference<WorldRoot> worldRoot_);
	Reference<WorldRoot> world_root_mut() const;

private:
	void detach_child(Reference<WorldInstance> child);
	void attach_child(Reference<WorldInstance> child);
	void recalculate_depth();

protected:
	Transform3D transform{}; // Transform
	Hierarchy hierarchy{ affine }; // Hierarchy

private:
	Affine affine;

	Reference<WorldRoot> worldRoot;

	u32 hierarchyDepth{ 0 };

	u64 instanceId;

protected:
	bool isActive = true;
	bool isDestroy{ false };
};


}; // szg
