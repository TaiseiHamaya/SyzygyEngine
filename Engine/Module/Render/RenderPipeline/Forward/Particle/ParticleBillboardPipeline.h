#pragma once

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"
#include "Engine/Module/Render/RenderPipeline/BaseRenderPipeline.h"

namespace szg {

enum class PsoBlendMode : u32;

/// <summary>
/// ビルボード粒子専用の描画パイプライン
/// </summary>
class ParticleBillboardPipeline final : public BaseRenderPipeline {
public:
	ParticleBillboardPipeline();
	~ParticleBillboardPipeline() noexcept;

	SZG_CLASS_MOVE_ONLY(ParticleBillboardPipeline)

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void initialize() override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void initialize(BlendMode blendMode);

	void preprocess() override {};

private:
	/// <summary>
	/// パイプライン生成
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void create_pipeline_state(PsoBlendMode blendMode);
};

}; // szg
