#pragma once

#include <memory>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

#include "../WorldInstance/WorldInstance.h"
#include "Engine/Runtime/Clock/WorldTimer.h"
#include "Engine/Runtime/Particle/EmitterSettings.h"

namespace szg {

class ParticleEmissionDirection;
class ParticleEmissionShape;
class ParticlePool;

/// <summary>
/// ParticleInstanceの放出を行う実行単位
/// </summary>
class EmitterInstance : public WorldInstance {
#ifdef DEBUG_FEATURES_ENABLE
	friend class RemoteEmitterInstance;
#endif // DEBUG_FEATURES_ENABLE

public:
	EmitterInstance();
	~EmitterInstance() override;

	SZG_CLASS_MOVE_ONLY(EmitterInstance)

public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void update() override;

	/// <summary>
	/// 破棄時の後処理
	/// </summary>
	void on_mark_destroy() override {};

public:
	/// <summary>
	/// 対プールの設定
	/// </summary>
	/// <param name="pool_">対プール</param>
	void setup_pool(Reference<ParticlePool> pool_);

	/// <summary>
	/// 設定の適用
	/// </summary>
	/// <param name="settings_">エミッタ設定</param>
	void setup_settings(const EmitterInstanceSettings& settings_);

	/// <summary>
	/// 設定の取得
	/// </summary>
	/// <returns></returns>
	const EmitterInstanceSettings& settings_imm() const { return settings; }

	/// <summary>
	/// 対プールの取得
	/// </summary>
	/// <returns></returns>
	Reference<ParticlePool> pool_mut() { return pool; }

	/// <summary>
	/// 放出スケジュールの初期化
	/// </summary>
	void restart_schedule();

	/// <summary>
	/// 更新者マスクの構築
	/// </summary>
	/// <param name="settings_">エミッタ設定</param>
	/// <returns></returns>
	static u32 BuildUpdaterMask(const EmitterInstanceSettings& settings_);

private:
	/// <summary>
	/// 放出形状に応じた初期位置の決定
	/// </summary>
	void emit();

	/// <summary>
	/// 放出方式の再構築
	/// </summary>
	void rebuild_emission();

private:
	EmitterInstanceSettings settings{};
	Reference<ParticlePool> pool;
	std::unique_ptr<ParticleEmissionShape> emissionShape;
	std::unique_ptr<ParticleEmissionDirection> emissionDirection;
	WorldTimer emitTimer{};
	r32 nextEmitTime{ 0.0f };
	u32 emittedCycles{ 0 };
};

}; // szg