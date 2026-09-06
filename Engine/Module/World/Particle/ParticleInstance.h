#pragma once

#include "./ParticleUpdaters/ParticleUpdater.h"

#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

/// <summary>
/// パーティクル1粒分の論理パラメータをid経由で参照する実行単位
/// </summary>
class ParticleInstance {
public:
	ParticleInstance() = default;
	~ParticleInstance() = default;

	SZG_CLASS_DEFAULT(ParticleInstance)

public:
	/// <summary>
	/// 全idの無効化
	/// </summary>
	void reset();

	/// <summary>
	/// 有効な寿命idを持つか
	/// </summary>
	/// <returns></returns>
	bool is_valid() const { return lifetime.is_valid(); }

public:
	ParticleId lifetime{};
	ParticleId pose{};
	ParticleId move{};
	ParticleId drag{};
	ParticleId color{};
	ParticleId scale{};
	ParticleId rotation{};
	ParticleId uvScroll{};
	ParticleId flipbook{};
	ParticleId alpha{};
};

}; // szg
