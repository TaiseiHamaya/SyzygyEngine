#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include "../BaseDrawExecutor.h"

#include <Library/Utility/Template/Reference.h>

#include "Engine/Assets/PolygonMesh/PolygonMesh.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/StructuredBuffer/StructuredBuffer.h"

#include "./ParticleDrawItem.h"

namespace szg {

/// <summary>
/// メッシュ粒子専用の描画実行器
/// </summary>
class ParticleMeshDrawExecutor final : public BaseDrawExecutor<ParticleDrawItem> {
public:
	ParticleMeshDrawExecutor() noexcept = default;
	~ParticleMeshDrawExecutor() noexcept = default;

	SZG_CLASS_MOVE_ONLY(ParticleMeshDrawExecutor)

public:
	/// <summary>
	/// 再初期化
	/// </summary>
	/// <param name="asset_">メッシュ資産</param>
	/// <param name="type_">ブレンドモード</param>
	/// <param name="maxInstance_">上限数</param>
	void reinitialize(std::shared_ptr<const PolygonMesh> asset_, BlendMode type_, u32 maxInstance_);
	void draw_command() const override;
	void write_to_buffer(Reference<const ParticleDrawItem> instance) override;

private:
	std::shared_ptr<const PolygonMesh> asset;
	BlendMode type{ BlendMode::Alpha };

	StructuredBuffer<TransformMatrixDataBuffer> matrices;
	std::vector<StructuredBuffer<MaterialDataBufferRGBA>> materials;

	std::mutex writeBufferMutex;
};

}; // szg
