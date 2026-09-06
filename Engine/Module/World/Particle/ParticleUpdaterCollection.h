#pragma once

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

#include "./ParticleInstance.h"

#include "./ParticleUpdaters/AlphaFadeUpdater.h"
#include "./ParticleUpdaters/AxisBillboardRotationUpdater.h"
#include "./ParticleUpdaters/BillboardRotationUpdater.h"
#include "./ParticleUpdaters/ColorUpdater.h"
#include "./ParticleUpdaters/DragGravityUpdater.h"
#include "./ParticleUpdaters/FlipbookUpdater.h"
#include "./ParticleUpdaters/LifetimeUpdater.h"
#include "./ParticleUpdaters/MoveUpdater.h"
#include "./ParticleUpdaters/PoseUpdater.h"
#include "./ParticleUpdaters/RandomRotationUpdater.h"
#include "./ParticleUpdaters/ScaleUpdater.h"
#include "./ParticleUpdaters/UVScrollUpdater.h"
#include "./ParticleUpdaters/VelocityAlignUpdater.h"

namespace szg {

class ParticlePool;
class WorldRenderCollection;

/// <summary>
/// ワールド共有の更新者群
/// </summary>
class ParticleUpdaterCollection {
public:
	ParticleUpdaterCollection() = default;
	~ParticleUpdaterCollection() = default;

	SZG_CLASS_MOVE_ONLY(ParticleUpdaterCollection)

public:
	/// <summary>
	/// 参照先の設定
	/// </summary>
	/// <param name="renderCollection_">描画収集</param>
	void setup(Reference<WorldRenderCollection> renderCollection_);

	/// <summary>
	/// 全更新者の登録解除
	/// </summary>
	void clear_all();

	/// <summary>
	/// プール内の稼働粒を更新する
	/// </summary>
	/// <param name="pool">対象プール</param>
	void update_pool(Reference<ParticlePool> pool);

public:
	LifetimeUpdater& lifetime_mut() { return lifetime; }
	const LifetimeUpdater& lifetime_imm() const { return lifetime; }
	PoseUpdater& pose_mut() { return pose; }
	const PoseUpdater& pose_imm() const { return pose; }
	MoveUpdater& move_mut() { return move; }
	const MoveUpdater& move_imm() const { return move; }
	DragGravityUpdater& drag_mut() { return drag; }
	const DragGravityUpdater& drag_imm() const { return drag; }
	ColorUpdater& color_mut() { return color; }
	const ColorUpdater& color_imm() const { return color; }
	ScaleUpdater& scale_mut() { return scale; }
	const ScaleUpdater& scale_imm() const { return scale; }
	BillboardRotationUpdater& billboard_mut() { return billboard; }
	const BillboardRotationUpdater& billboard_imm() const { return billboard; }
	VelocityAlignUpdater& velocity_mut() { return velocity; }
	const VelocityAlignUpdater& velocity_imm() const { return velocity; }
	RandomRotationUpdater& random_mut() { return random; }
	const RandomRotationUpdater& random_imm() const { return random; }
	AxisBillboardRotationUpdater& axis_billboard_mut() { return axisBillboard; }
	const AxisBillboardRotationUpdater& axis_billboard_imm() const { return axisBillboard; }
	UVScrollUpdater& uv_mut() { return uvScroll; }
	const UVScrollUpdater& uv_imm() const { return uvScroll; }
	FlipbookUpdater& flip_mut() { return flipbook; }
	const FlipbookUpdater& flip_imm() const { return flipbook; }
	AlphaFadeUpdater& alpha_mut() { return alphaFade; }
	const AlphaFadeUpdater& alpha_imm() const { return alphaFade; }

private:
	/// <summary>
	/// 1粒の更新
	/// </summary>
	/// <param name="instance">対象粒子</param>
	/// <param name="mask">更新者マスク</param>
	/// <param name="rotation">回転選択</param>
	void update_particle(ParticleInstance& instance, u32 mask, ParticleRotationKind rotation);

private:
	LifetimeUpdater lifetime;
	PoseUpdater pose;
	MoveUpdater move;
	DragGravityUpdater drag;
	ColorUpdater color;
	ScaleUpdater scale;
	BillboardRotationUpdater billboard;
	VelocityAlignUpdater velocity;
	RandomRotationUpdater random;
	AxisBillboardRotationUpdater axisBillboard;
	UVScrollUpdater uvScroll;
	FlipbookUpdater flipbook;
	AlphaFadeUpdater alphaFade;

	Reference<WorldRenderCollection> renderCollection;
};

}; // szg
