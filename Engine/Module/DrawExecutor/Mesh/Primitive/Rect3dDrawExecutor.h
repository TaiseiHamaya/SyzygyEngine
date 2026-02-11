#pragma once

#include <mutex>

#include "../../BaseDrawExecutor.h"

#include <Library/Utility/Template/Reference.h>

#include "Engine/GraphicsAPI/DirectX/DxResource/StructuredBuffer/StructuredBuffer.h"
#include "Engine/Module/World/Mesh/Primitive/IPrimitiveInstance.h"

namespace szg {

class Rect3d;
struct RectData;

class Rect3dDrawExecutor final : public BaseDrawExecutor<Rect3d> {
public:
	Rect3dDrawExecutor() noexcept = default;
	~Rect3dDrawExecutor() noexcept = default;

	SZG_CLASS_MOVE_ONLY(Rect3dDrawExecutor)

public:
	void reinitialize(BlendMode type_, u32 maxInstance_);
	void draw_command() const override;
	void write_to_buffer(Reference<const Rect3d> instance) override;

private:
	BlendMode type;

	StructuredBuffer<TransformMatrixDataBuffer> matrices;
	StructuredBuffer<RectData> rectData;
	StructuredBuffer<MaterialDataBufferRGBA> material;

	std::mutex writeBufferMutex;
};

}; // szg
