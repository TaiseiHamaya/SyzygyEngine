#pragma once

#include "Engine/Module/Render/RenderPipeline/BaseRenderPipeline.h"

#include "../DeferredAdaptor.h"

namespace szg {

class PointLightingPipeline final : public BaseRenderPipeline {
public:
	PointLightingPipeline();
	~PointLightingPipeline() noexcept;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void initialize() override;

	void preprocess() override {};

private:
	void create_pipeline_state();
};


}; // szg
