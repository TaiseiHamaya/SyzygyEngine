#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <string>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class ParticlePool;

/// <summary>
/// プールの参照表示
/// </summary>
class RemoteParticlePoolView {
public:
	RemoteParticlePoolView() = default;
	~RemoteParticlePoolView() = default;

	SZG_CLASS_DEFAULT(RemoteParticlePoolView)

public:
	/// <summary>
	/// 表示値の更新
	/// </summary>
	/// <param name="pool">対象プール</param>
	void refresh(Reference<ParticlePool> pool);

	/// <summary>
	/// インスペクタ表示
	/// </summary>
	void draw_inspector() const;

	/// <summary>
	/// 稼働数
	/// </summary>
	/// <returns></returns>
	u32 alive_count() const { return aliveCount; }

	/// <summary>
	/// 上限数
	/// </summary>
	/// <returns></returns>
	u32 max_count() const { return maxCount; }

private:
	u32 aliveCount{ 0 };
	u32 maxCount{ 0 };
	std::string overflowPolicyName{ "Unknown" };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
