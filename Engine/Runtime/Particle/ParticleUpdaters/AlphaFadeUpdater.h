#pragma once

#include "./ParticleUpdater.h"

#include <Library/Math/ColorRGBA.h>

namespace szg {

/// <summary>
/// フェード実体
/// </summary>
struct AlphaFadePayload {
	r32 fadeIn{ 0.0f };
	r32 fadeOut{ 0.0f };
};

/// <summary>
/// 寿命媒介変数に応じたフェードイン・フェードアウト
/// </summary>
class AlphaFadeUpdater final : public ParticleUpdaterTable<AlphaFadePayload> {
public:
	using Payload = AlphaFadePayload;

public:
	AlphaFadeUpdater() = default;
	~AlphaFadeUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(AlphaFadeUpdater)

public:
	/// <summary>
	/// フェードの登録
	/// </summary>
	/// <param name="fadeIn_">フェードイン時間</param>
	/// <param name="fadeOut_">フェードアウト時間</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_fade(r32 fadeIn_, r32 fadeOut_);

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <param name="parametric_">寿命媒介変数</param>
	/// <param name="color">ColorUpdater実体への参照</param>
	void update(ParticleId id, r32 parametric_, ColorRGBA& color);
};

}; // szg
