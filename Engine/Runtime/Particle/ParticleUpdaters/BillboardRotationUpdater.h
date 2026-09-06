#pragma once

#include "./ParticleUpdater.h"

#include <Library/Math/Quaternion.h>
#include <Library/Math/Vector3.h>
#include <Library/Utility/Template/Reference.h>

namespace szg {

class CameraInstance;

/// <summary>
/// 無効なカメラID
/// </summary>
constexpr u32 LOOKAT_INVALID_CAMERA_ID = 0xFFFFFFFFu;

/// <summary>
/// 面内回転＋注視実体
/// </summary>
struct BillboardRotationPayload {
	r32 angle{ 0.0f };
	r32 angularVelocity{ 0.0f };
	Vector3 target{ CVector3::ZERO };
	u32 cameraId{ LOOKAT_INVALID_CAMERA_ID };
};

/// <summary>
/// 板の面内回転＋指定対象への注視
/// </summary>
class BillboardRotationUpdater final : public ParticleUpdaterTable<BillboardRotationPayload> {
public:
	using Payload = BillboardRotationPayload;

public:
	BillboardRotationUpdater() = default;
	~BillboardRotationUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(BillboardRotationUpdater)

public:
	/// <summary>
	/// 面内回転＋注視の登録
	/// </summary>
	/// <param name="angle_">初期角</param>
	/// <param name="angularVelocity_">角速度</param>
	/// <param name="target_">注視点</param>
	/// <param name="cameraId_">注視カメラID</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_rotation(r32 angle_, r32 angularVelocity_, const Vector3& target_, u32 cameraId_);

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <param name="position">PoseUpdater実体への参照</param>
	/// <param name="rotation">PoseUpdater実体への参照</param>
	/// <param name="target">解決済み注視点</param>
	/// <param name="cameraInstance">対象カメラインスタンス。無効な場合はtargetへLookForwardに退避</param>
	void update(ParticleId id, const Vector3& position, Quaternion& rotation, Reference<const CameraInstance> cameraInstance);
};

}; // szg
