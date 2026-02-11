#pragma once

#include <mutex>
#include <vector>

#include "../../BaseDrawExecutor.h"

#include <Library/Utility/Template/Reference.h>

#include "Engine/Assets/PolygonMesh/PolygonMesh.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/StructuredBuffer/StructuredBuffer.h"

namespace szg {

class StaticMeshInstance;
struct TransformMatrixDataBuffer;
struct MaterialDataBufferRGB;

class StaticMeshDrawExecutorForward  final : public BaseDrawExecutor<StaticMeshInstance> {
public:
	StaticMeshDrawExecutorForward() noexcept = default;
	~StaticMeshDrawExecutorForward() noexcept = default;

	StaticMeshDrawExecutorForward(std::shared_ptr<const PolygonMesh> asset_, u32 maxInstance);

	SZG_CLASS_MOVE_ONLY(StaticMeshDrawExecutorForward)

public:
	void reinitialize(std::shared_ptr<const PolygonMesh> asset_, u32 maxInstance);
	void draw_command() const override;
	void write_to_buffer(Reference<const StaticMeshInstance> instance) override;

private:
	std::shared_ptr<const PolygonMesh> asset;

	StructuredBuffer<TransformMatrixDataBuffer> matrices;
	std::vector<StructuredBuffer<MaterialDataBufferRGB>> materials;

	std::mutex writeBufferMutex;
};

}; // szg
