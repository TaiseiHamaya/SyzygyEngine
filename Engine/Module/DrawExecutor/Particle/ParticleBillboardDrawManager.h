#pragma once

#include "../BaseDrawManager.h"
#include "./ParticleBillboardDrawExecutor.h"

namespace szg {

/// <summary>
/// ビルボード粒子専用の描画管理
/// </summary>
class ParticleBillboardDrawManager : public BaseDrawManager<ParticleBillboardDrawExecutor, BlendMode, ParticleDrawItem> {
public:
	ParticleBillboardDrawManager() = default;
	~ParticleBillboardDrawManager() override = default;

	SZG_CLASS_MOVE_ONLY(ParticleBillboardDrawManager)

public:
	void make_instancing(u32 layer, const BlendMode& blendMode, u32 maxInstance) override;

	/// <summary>
	/// 書込先の確保
	/// </summary>
	/// <param name="layer">レイヤー</param>
	/// <param name="blendMode">ブレンドモード</param>
	/// <param name="maxInstance">上限数</param>
	void ensure_instancing(u32 layer, const BlendMode& blendMode, u32 maxInstance);

	/// <summary>
	/// 1件の書込
	/// </summary>
	/// <param name="item">描画項目</param>
	void transfer_item(Reference<const ParticleDrawItem> item);
};

}; // szg
