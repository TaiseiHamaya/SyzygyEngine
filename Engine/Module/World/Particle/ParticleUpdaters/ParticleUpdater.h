#pragma once

#include <vector>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

/// <summary>
/// 粒子パラメータの世代付きハンドル
/// </summary>
struct ParticleId {
	static constexpr u32 INVALID_INDEX = 0xFFFFFFFFu;

	u32 index{ INVALID_INDEX };
	u32 generation{ 0 };

	/// <summary>
	/// 有効な参照か
	/// </summary>
	/// <returns></returns>
	bool is_valid() const { return index != INVALID_INDEX; }

	bool operator==(const ParticleId& rhs) const { return index == rhs.index && generation == rhs.generation; }
	bool operator!=(const ParticleId& rhs) const { return !(*this == rhs); }
};

/// <summary>
/// parametric補間に用いるイージング選択
/// </summary>
enum class ParticleEasingMode : u32 {
	Linear,
	In,
	Out,
	InOut,
};

/// <summary>
/// 更新者の種別
/// </summary>
enum class ParticleUpdaterKind : u32 {
	Lifetime = 1u << 0,
	Pose = 1u << 1,
	Move = 1u << 2,
	DragGravity = 1u << 3,
	Color = 1u << 4,
	Scale = 1u << 5,
	Rotation = 1u << 6,
	UVScroll = 1u << 7,
	Flipbook = 1u << 8,
	AlphaFade = 1u << 9,
};

/// <summary>
/// 回転更新者の選択
/// </summary>
enum class ParticleRotationKind : u32 {
	Billboard,
	Constant,
	VelocityAlign,
	Random,
	AxisBillboard,
};

/// <summary>
/// 粒子パラメータ更新者の基底
/// </summary>
class ParticleUpdater {
public:
	ParticleUpdater() = default;
	virtual ~ParticleUpdater() = default;

	SZG_CLASS_MOVE_ONLY(ParticleUpdater)

public:
	/// <summary>
	/// 全スロットの登録解除
	/// </summary>
	virtual void clear() = 0;

	/// <summary>
	/// 稼働数
	/// </summary>
	/// <returns></returns>
	virtual u32 alive_count() const = 0;
};

/// <summary>
/// 世代付きハンドル＋free-listによる実体表
/// </summary>
/// <typeparam name="Payload">更新者ごとの実体型</typeparam>
template<typename Payload>
class ParticleUpdaterTable : public ParticleUpdater {
public:
	struct Slot {
		u32 generation{ 0 };
		bool alive{ false };
		Payload payload{};
	};

public:
	ParticleUpdaterTable() = default;
	~ParticleUpdaterTable() override = default;

	SZG_CLASS_MOVE_ONLY(ParticleUpdaterTable)

public:
	/// <summary>
	/// 実体の登録
	/// </summary>
	/// <param name="initial">初期値</param>
	/// <returns>世代付きハンドル</returns>
	ParticleId register_slot(const Payload& initial);

	/// <summary>
	/// 実体の登録解除
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	void unregister(ParticleId id);

	/// <summary>
	/// idから実体への解決
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <returns>無効なidの場合は空参照</returns>
	Reference<Payload> resolve(ParticleId id);

	/// <summary>
	/// idから実体への解決
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <returns>無効なidの場合は空参照</returns>
	Reference<const Payload> resolve(ParticleId id) const;

	/// <summary>
	/// 全スロットの登録解除
	/// </summary>
	void clear() override;

	/// <summary>
	/// 稼働数
	/// </summary>
	/// <returns></returns>
	u32 alive_count() const override;

	/// <summary>
	/// 枠の事前確保
	/// </summary>
	/// <param name="capacity">確保数</param>
	void reserve(u32 capacity);

private:
	/// <summary>
	/// 現世代の有効なidか
	/// </summary>
	/// <param name="id">世代付きハンドル</param>
	/// <returns></returns>
	bool is_current(ParticleId id) const;

private:
	std::vector<Slot> slots;
	std::vector<u32> freeList;
	u32 aliveCount{ 0 };
};

}; // szg

#include "ParticleUpdaterTable.inl"
