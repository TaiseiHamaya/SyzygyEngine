#pragma once

#include "./ParticleUpdater.h"

#include <Library/Math/Quaternion.h>
#include <Library/Math/Vector3.h>
#include <Library/Utility/Template/Reference.h>

#include "./BillboardRotationUpdater.h"

namespace szg {

class CameraInstance;

/// <summary>
/// 軸ビルボード実体
/// </summary>
struct AxisBillboardRotationPayload {
	Vector3 axis{ CVector3::BASIS_Y };
	r32 angle{ 0.0f };
	Vector3 target{ CVector3::ZERO };
	u32 cameraId{ LOOKAT_INVALID_CAMERA_ID };
};

/// <summary>
/// 指定軸周りの注視。WorldInstance::look_at_axisの流儀
/// </summary>
class AxisBillboardRotationUpdater final : public ParticleUpdaterTable<AxisBillboardRotationPayload> {
public:
	using Payload = AxisBillboardRotationPayload;

public:
	AxisBillboardRotationUpdater() = default;
	~AxisBillboardRotationUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(AxisBillboardRotationUpdater)

public:
	/// <summary>
	/// 軸ビルボードの登録
	/// </summary>
	/// <param name="axis_">回転軸</param>
	/// <param name="angle_">面内角</param>
	/// <param name="target_">注視点</param>
	/// <param name="cameraId_">注視カメラID</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_rotation(const Vector3& axis_, r32 angle_, const Vector3& target_, u32 cameraId_);

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <param name="position">PoseUpdater実体への参照</param>
	/// <param name="rotation">PoseUpdater実体への参照</param>
	/// <param name="cameraInstance">対象カメラインスタンス。無効な場合はpayloadのtargetを使用</param>
	void update(ParticleId id, const Vector3& position, Quaternion& rotation, Reference<const CameraInstance> cameraInstance);
};

}; // szg
