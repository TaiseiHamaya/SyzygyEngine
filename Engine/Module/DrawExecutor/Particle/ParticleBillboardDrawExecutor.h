#pragma once

#include <mutex>

#include "../BaseDrawExecutor.h"

#include <Library/Math/Vector2.h>
#include <Library/Utility/Template/Reference.h>

#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/StructuredBuffer/StructuredBuffer.h"

#include "./ParticleDrawItem.h"

namespace szg {

/// <summary>
/// ビルボード粒子専用の矩形データ
/// </summary>
struct ParticleBillboardDataBuffer {
	Vector2 size{ CVector2::BASIS };
	Vector2 pivot{ 0.5f, 0.5f };
	r32 angle{ 0.0f };
	r32 padding{ 0.0f };
};

/// <summary>
/// ビルボード粒子専用の描画実行器
/// </summary>
class ParticleBillboardDrawExecutor final : public BaseDrawExecutor<ParticleDrawItem> {
public:
	ParticleBillboardDrawExecutor() noexcept = default;
	~ParticleBillboardDrawExecutor() noexcept = default;

	SZG_CLASS_MOVE_ONLY(ParticleBillboardDrawExecutor)

public:
	/// <summary>
	/// 再初期化
	/// </summary>
	/// <param name="type_">ブレンドモード</param>
	/// <param name="maxInstance_">上限数</param>
	void reinitialize(BlendMode type_, u32 maxInstance_);
	void draw_command() const override;
	void write_to_buffer(Reference<const ParticleDrawItem> instance) override;

private:
	BlendMode type{ BlendMode::Alpha };

	StructuredBuffer<TransformMatrixDataBuffer> matrices;
	StructuredBuffer<ParticleBillboardDataBuffer> rectData;
	StructuredBuffer<MaterialDataBufferRGBA> material;

	std::mutex writeBufferMutex;
};

}; // szg
