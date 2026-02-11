#pragma once

#include "./ParticleBufferValue.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/StructuredBuffer/StructuredBuffer.h"

#include <Library/Utility/Tools/ConstructorMacro.h>

enum class ParticleDrawType {
	Mesh,
	Rect,
};

namespace szg {

class BaseParticleDrawSystem {
public:
	BaseParticleDrawSystem() = default;
	virtual ~BaseParticleDrawSystem() = default;

	SZG_CLASS_MOVE_ONLY(BaseParticleDrawSystem)

public:
	virtual void draw_command(size_t InstanceCount) const = 0;
	void create_buffers(u32 bufferSize);
	void write_to_buffer(u32 index, const Matrix4x4& worldMatrix, const Matrix4x4& uvMatrix, const ColorRGBA& color);

protected:
	StructuredBuffer<ParticleBuffer> particleBuffer;
};

}; // szg
