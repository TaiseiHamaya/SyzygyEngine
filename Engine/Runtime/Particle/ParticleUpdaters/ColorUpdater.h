#pragma once

#include "./ParticleUpdater.h"

#include <Library/Math/ColorRGBA.h>

namespace szg {

constexpr u32 COLOR_UPDATER_MAX_KEYS = 4;

/// <summary>
/// 色キー
/// </summary>
struct ColorKey {
	r32 time{ 0.0f };
	ColorRGBA color{ CColorRGBA::WHITE };
};

/// <summary>
/// 色実体
/// </summary>
struct ColorPayload {
	ColorRGBA current{ CColorRGBA::WHITE };
	ColorKey keys[COLOR_UPDATER_MAX_KEYS]{};
	u32 keyCount{ 0 };
	ParticleEasingMode easing{ ParticleEasingMode::Linear };
};

/// <summary>
/// 開始色→終了色の補間。多段カーブ対応
/// </summary>
class ColorUpdater final : public ParticleUpdaterTable<ColorPayload> {
public:
	using Payload = ColorPayload;
	using Key = ColorKey;
	static constexpr u32 MAX_KEYS = COLOR_UPDATER_MAX_KEYS;

public:
	ColorUpdater() = default;
	~ColorUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(ColorUpdater)

public:
	/// <summary>
	/// 色の登録
	/// </summary>
	/// <param name="start">開始色</param>
	/// <param name="end">終了色</param>
	/// <param name="easing_">イージング選択</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_color(const ColorRGBA& start, const ColorRGBA& end, ParticleEasingMode easing_ = ParticleEasingMode::Linear);

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <param name="parametric_">寿命媒介変数</param>
	void update(ParticleId id, r32 parametric_);

	/// <summary>
	/// 中間キーの設定
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <param name="keyIndex">キー番号</param>
	/// <param name="time_">時刻</param>
	/// <param name="color_">色</param>
	void set_key(ParticleId id, u32 keyIndex, r32 time_, const ColorRGBA& color_);

private:
	/// <summary>
	/// イージング適用
	/// </summary>
	/// <param name="t">媒介変数</param>
	/// <param name="easing_">イージング選択</param>
	/// <returns></returns>
	static r32 apply_easing(r32 t, ParticleEasingMode easing_);
};

}; // szg
