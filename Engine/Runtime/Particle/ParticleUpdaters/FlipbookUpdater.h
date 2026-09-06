#pragma once

#include "./ParticleUpdater.h"

#include "Engine/Runtime/Clock/WorldTimer.h"

namespace szg {

/// <summary>
/// フリップブック実体
/// </summary>
struct FlipbookPayload {
	u32 columns{ 1 };
	u32 rows{ 1 };
	r32 frameTime{ 0.1f };
	bool isLoop{ true };
	WorldTimer timer{};
	u32 index{ 0 };
};

/// <summary>
/// テクスチャアトラスのコマ送り
/// </summary>
class FlipbookUpdater final : public ParticleUpdaterTable<FlipbookPayload> {
public:
	using Payload = FlipbookPayload;

public:
	FlipbookUpdater() = default;
	~FlipbookUpdater() override = default;

	SZG_CLASS_MOVE_ONLY(FlipbookUpdater)

public:
	/// <summary>
	/// フリップブックの登録
	/// </summary>
	/// <param name="columns_">列数</param>
	/// <param name="rows_">行数</param>
	/// <param name="frameTime_">1コマ時間</param>
	/// <param name="isLoop_">ループ有無</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_flipbook(u32 columns_, u32 rows_, r32 frameTime_, bool isLoop_);

	/// <summary>
	/// 単一更新エントリ
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	void update(ParticleId id);
};

}; // szg
