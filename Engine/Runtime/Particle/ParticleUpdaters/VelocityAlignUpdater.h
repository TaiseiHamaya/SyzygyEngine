#pragma once

#include "./ParticleUpdater.h"

#include <Library/Math/Quaternion.h>
#include <Library/Math/Vector3.h>

namespace szg {

/// <summary>
/// 速度合わせ実体
/// </summary>
struct VelocityAlignPayload {
	u8 reserved{ 0 };
};

/// <summary>
/// 速度方向への姿勢合わせ
/// </summary>
class VelocityAlignUpdater final : public ParticleUpdaterTable<VelocityAlignPayload> {
public:
	using Payload = VelocityAlignPayload;

public:
	VelocityAlignUpdater() = default;
	~VelocityAlignUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(VelocityAlignUpdater)

public:
	/// <summary>
	/// 速度合わせの登録
	/// </summary>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_rotation();

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <param name="velocity">MoveUpdater実体への参照</param>
	/// <param name="rotation">PoseUpdater実体への参照</param>
	void update(ParticleId id, const Vector3& velocity, Quaternion& rotation);
};

}; // szg
