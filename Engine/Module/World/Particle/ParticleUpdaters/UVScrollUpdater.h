#pragma once

#include "./ParticleUpdater.h"

#include <Library/Math/Vector2.h>

namespace szg {

/// <summary>
/// UVスクロール実体
/// </summary>
struct UVScrollPayload {
	Vector2 offset{ CVector2::ZERO };
	Vector2 speed{ CVector2::ZERO };
};

/// <summary>
/// UVオフセットのスクロール
/// </summary>
class UVScrollUpdater final : public ParticleUpdaterTable<UVScrollPayload> {
public:
	using Payload = UVScrollPayload;

public:
	UVScrollUpdater() = default;
	~UVScrollUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(UVScrollUpdater)

public:
	/// <summary>
	/// UVスクロールの登録
	/// </summary>
	/// <param name="offset_">初期オフセット</param>
	/// <param name="speed_">速度</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_uv(const Vector2& offset_, const Vector2& speed_);

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	void update(ParticleId id);
};

}; // szg
