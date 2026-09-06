#pragma once

#include <memory>
#include <string>

#include "../BaseDrawManager.h"
#include "./ParticleMeshDrawExecutor.h"

namespace szg {

class PolygonMesh;

/// <summary>
/// メッシュ粒子専用の描画管理
/// </summary>
class ParticleMeshDrawManager : public BaseDrawManager<ParticleMeshDrawExecutor, std::string, ParticleDrawItem> {
public:
	ParticleMeshDrawManager() = default;
	~ParticleMeshDrawManager() override = default;

	SZG_CLASS_MOVE_ONLY(ParticleMeshDrawManager)

public:
	void make_instancing(u32 layer, const std::string& meshName, u32 maxInstance) override;

	/// <summary>
	/// 書込先の確保
	/// </summary>
	/// <param name="layer">レイヤー</param>
	/// <param name="meshName_">メッシュ名</param>
	/// <param name="blendMode">ブレンドモード</param>
	/// <param name="maxInstance">上限数</param>
	/// <param name="asset">メッシュ資産</param>
	void ensure_instancing(u32 layer, const std::string& meshName_, BlendMode blendMode, u32 maxInstance, std::shared_ptr<const PolygonMesh> asset);

	/// <summary>
	/// 1件の書込
	/// </summary>
	/// <param name="item">描画項目</param>
	void transfer_item(Reference<const ParticleDrawItem> item);

	/// <summary>
	/// 管理キーの構築
	/// </summary>
	/// <param name="meshName_">メッシュ名</param>
	/// <param name="blendMode">ブレンドモード</param>
	/// <returns></returns>
	static std::string MakeKey(const std::string& meshName_, BlendMode blendMode);
};

}; // szg
