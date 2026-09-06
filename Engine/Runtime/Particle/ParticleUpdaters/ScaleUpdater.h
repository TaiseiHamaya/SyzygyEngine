#pragma once

#include "./ParticleUpdater.h"

#include <Library/Math/Vector3.h>

namespace szg {

/// <summary>
/// サイズ実体
/// </summary>
struct ScalePayload {
	Vector3 startScale{ CVector3::ONE };
	Vector3 endScale{ CVector3::ONE };
	ParticleEasingMode easing{ ParticleEasingMode::Linear };
};

/// <summary>
/// 開始サイズ→終了サイズの補間
/// </summary>
class ScaleUpdater final : public ParticleUpdaterTable<ScalePayload> {
public:
	using Payload = ScalePayload;

public:
	ScaleUpdater() = default;
	~ScaleUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(ScaleUpdater)

public:
	/// <summary>
	/// サイズの登録
	/// </summary>
	/// <param name="start_">開始サイズ</param>
	/// <param name="end_">終了サイズ</param>
	/// <param name="easing_">イージング選択</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_scale(const Vector3& start_, const Vector3& end_, ParticleEasingMode easing_ = ParticleEasingMode::Linear);

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <param name="parametric_">寿命媒介変数</param>
	/// <param name="scale">PoseUpdater実体への参照</param>
	void update(ParticleId id, r32 parametric_, Vector3& scale);
};

}; // szg
