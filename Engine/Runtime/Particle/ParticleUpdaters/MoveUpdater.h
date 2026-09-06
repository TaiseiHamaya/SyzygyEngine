#pragma once

#include "./ParticleUpdater.h"

#include <Library/Math/Vector3.h>

namespace szg {

/// <summary>
/// 運動実体
/// </summary>
struct MovePayload {
	Vector3 velocity{ CVector3::ZERO };
	Vector3 acceleration{ CVector3::ZERO };
};

/// <summary>
/// 速度＋加速度による移動
/// </summary>
class MoveUpdater final : public ParticleUpdaterTable<MovePayload> {
public:
	using Payload = MovePayload;

public:
	MoveUpdater() = default;
	~MoveUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(MoveUpdater)

public:
	/// <summary>
	/// 運動の登録
	/// </summary>
	/// <param name="velocity_">速度</param>
	/// <param name="acceleration_">加速度</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_move(const Vector3& velocity_, const Vector3& acceleration_);

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <param name="position">PoseUpdater実体への参照</param>
	void update(ParticleId id, Vector3& position);
};

}; // szg
