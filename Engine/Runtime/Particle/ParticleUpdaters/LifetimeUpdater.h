#pragma once

#include "./ParticleUpdater.h"

#include "Engine/Runtime/Clock/WorldTimer.h"

namespace szg {

/// <summary>
/// 寿命実体
/// </summary>
struct LifetimePayload {
	WorldTimer timer{};
	r32 lifetime{ 1.0f };
};

/// <summary>
/// 寿命進行と終了判定
/// </summary>
class LifetimeUpdater final : public ParticleUpdaterTable<LifetimePayload> {
public:
	using Payload = LifetimePayload;

public:
	LifetimeUpdater() = default;
	~LifetimeUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(LifetimeUpdater)

public:
	/// <summary>
	/// 寿命の登録
	/// </summary>
	/// <param name="lifetime_">寿命</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_lifetime(r32 lifetime_);

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	void update(ParticleId id);

	/// <summary>
	/// 生存判定
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <returns></returns>
	bool is_alive(ParticleId id) const;

	/// <summary>
	/// 媒介変数
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <returns></returns>
	r32 parametric(ParticleId id) const;
};

}; // szg
