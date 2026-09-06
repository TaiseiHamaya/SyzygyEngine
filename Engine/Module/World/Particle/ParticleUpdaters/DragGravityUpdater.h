#pragma once

#include "./ParticleUpdater.h"

#include <Library/Math/Vector3.h>

namespace szg {

/// <summary>
/// 減衰・重力実体
/// </summary>
struct DragGravityPayload {
	r32 drag{ 0.0f };
	Vector3 gravity{ CVector3::ZERO };
};

/// <summary>
/// 減衰と重力の適用
/// </summary>
class DragGravityUpdater final : public ParticleUpdaterTable<DragGravityPayload> {
public:
	using Payload = DragGravityPayload;

public:
	DragGravityUpdater() = default;
	~DragGravityUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(DragGravityUpdater)

public:
	/// <summary>
	/// 減衰・重力の登録
	/// </summary>
	/// <param name="drag_">減衰係数</param>
	/// <param name="gravity_">重力加速度</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_drag_gravity(r32 drag_, const Vector3& gravity_);

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <param name="velocity">MoveUpdater実体への参照</param>
	void update(ParticleId id, Vector3& velocity);
};

}; // szg
