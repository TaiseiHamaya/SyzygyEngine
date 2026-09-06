#pragma once

#include "./ParticleUpdater.h"

#include <Library/Math/Quaternion.h>
#include <Library/Math/Vector3.h>

namespace szg {

/// <summary>
/// 姿勢実体
/// </summary>
struct PosePayload {
	Vector3 position{ CVector3::ZERO };
	Quaternion rotation{ CQuaternion::IDENTITY };
	Vector3 scale{ CVector3::ONE };
};

/// <summary>
/// 位置・回転・サイズの実体保持。姿勢の正本
/// </summary>
class PoseUpdater final : public ParticleUpdaterTable<PosePayload> {
public:
	using Payload = PosePayload;

public:
	PoseUpdater() = default;
	~PoseUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(PoseUpdater)

public:
	/// <summary>
	/// 姿勢の登録
	/// </summary>
	/// <param name="position_">位置</param>
	/// <param name="rotation_">回転</param>
	/// <param name="scale_">サイズ</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_pose(const Vector3& position_, const Quaternion& rotation_, const Vector3& scale_);

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	void update(ParticleId id);
};

}; // szg
