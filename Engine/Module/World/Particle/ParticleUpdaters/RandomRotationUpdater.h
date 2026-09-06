#pragma once

#include "./ParticleUpdater.h"

#include <Library/Math/Quaternion.h>
#include <Library/Math/Vector3.h>

namespace szg {

/// <summary>
/// ランダム回転実体
/// </summary>
struct RandomRotationPayload {
	Vector3 axis{ CVector3::BASIS_Y };
	r32 angularVelocity{ 0.0f };
};

/// <summary>
/// ランダム軸・角速度による回転。既定では無効のオプション扱い
/// </summary>
class RandomRotationUpdater final : public ParticleUpdaterTable<RandomRotationPayload> {
public:
	using Payload = RandomRotationPayload;

public:
	RandomRotationUpdater() = default;
	~RandomRotationUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(RandomRotationUpdater)

public:
	/// <summary>
	/// ランダム回転の登録
	/// </summary>
	/// <param name="axis_">回転軸</param>
	/// <param name="angularVelocity_">角速度</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_rotation(const Vector3& axis_, r32 angularVelocity_);

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <param name="rotation">PoseUpdater実体への参照</param>
	void update(ParticleId id, Quaternion& rotation);
};

}; // szg
