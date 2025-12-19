#pragma once

#include "../../BaseRenderPipeline.h"

namespace szg {

class GridPipeline final : public szg::BaseRenderPipeline {
public:
	GridPipeline() noexcept = default;
	~GridPipeline() noexcept = default;

	SZG_CLASS_MOVE_ONLY(GridPipeline)

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void initialize() override;

	/// <summary>
	/// 使用決定時処理
	/// </summary>
	void preprocess() {};

private:
	void create_pipeline_state();
};

}; // szg
