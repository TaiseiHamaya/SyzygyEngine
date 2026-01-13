#pragma once

#include <vector>
#include <mutex>

#include "../BaseDrawExecutor.h"

#include <Library/Utility/Template/Reference.h>

#include "Engine/Assets/PolygonMesh/PolygonMesh.h"
#include "Engine/Assets/Animation/Skeleton/SkeletonAsset.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/ConstantBuffer/ConstantBuffer.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/StructuredBuffer/StructuredBuffer.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/StructuredBuffer/MdStructuredBuffer.h"

namespace szg {

class SkinningMeshInstance;
struct TransformMatrixDataBuffer;
struct MaterialDataBufferRGB;
struct SkeletonMatrixPaletteWellBuffer;

class SkinningMeshDrawExecutor final : public BaseDrawExecutor<SkinningMeshInstance> {
public:
	SkinningMeshDrawExecutor() noexcept = default;
	~SkinningMeshDrawExecutor() noexcept = default;

	SkinningMeshDrawExecutor(std::shared_ptr<const PolygonMesh> mesh, std::shared_ptr<const SkeletonAsset> skeleton, u32 maxInstance);
	
	SZG_CLASS_MOVE_ONLY(SkinningMeshDrawExecutor)

public:
	void reinitialize(std::shared_ptr<const PolygonMesh> mesh, std::shared_ptr<const SkeletonAsset> skeleton, u32 maxInstance);
	void draw_command() const override;
	void write_to_buffer(Reference<const SkinningMeshInstance> instance) override;

private:
	std::shared_ptr<const PolygonMesh> mesh;
	std::shared_ptr<const SkeletonAsset> skeletonData;
	
	StructuredBuffer<TransformMatrixDataBuffer> matrices;
	std::vector<StructuredBuffer<MaterialDataBufferRGB>> materials;
	std::vector<MdStructuredBuffer<SkeletonMatrixPaletteWellBuffer>> matrixPalettes; // GPU用Matrix
	std::vector<ConstantBuffer<u32>> paletteSize;

	std::mutex writeBufferMutex;
};

}; // szg
