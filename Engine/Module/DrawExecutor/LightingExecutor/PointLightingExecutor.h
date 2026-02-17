#pragma once

#include "../BaseDrawExecutor.h"

#include "Engine/Assets/PrimitiveGeometry/PrimitiveGeometryAsset.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/StructuredBuffer/StructuredBuffer.h"
#include "Engine/Module/World/Light/PointLight/PointLightInstance.h"

#include <Library/Math/Matrix4x4.h>

namespace szg {

class PointLightingExecutor final : public BaseDrawExecutor<PointLightInstance> {
public:
	PointLightingExecutor() = default;
	~PointLightingExecutor() = default;

	PointLightingExecutor(u32 maxInstance);

	SZG_CLASS_MOVE_ONLY(PointLightingExecutor)

public:
	void reinitialize(u32 maxInstance);
	void draw_command() const override;
	void write_to_buffer(Reference<const PointLightInstance> instance) override;

private:
	std::shared_ptr<const PrimitiveGeometryAsset> asset;

	StructuredBuffer<Matrix4x4> matrices;
	StructuredBuffer<PointLightData> lightData;
};

}; // szg
