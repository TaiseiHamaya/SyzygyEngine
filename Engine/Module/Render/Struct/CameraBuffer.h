#pragma once

#include "Engine/GraphicsAPI/DirectX/DxResource/ConstantBuffer/ConstantBuffer.h"

#include <Library/Utility/Tools/ConstructorMacro.h>
#include <Library/Math/Matrix4x4.h>
#include <Library/Math/Vector3.h>

namespace szg {

class CameraInstance;

class CameraBuffer final {
public:
	struct VpBuffers {
		Matrix4x4 viewProjection;
		Matrix4x4 view;
	};

	struct LightingPathBuffer {
		Vector3 position;
		u32 padding{ 0 };
		Matrix4x4 viewInv;
		Matrix4x4 projInv;
	};

public:
	CameraBuffer() = default;
	~CameraBuffer() noexcept = default;

	SZG_CLASS_MOVE_ONLY(CameraBuffer)

public:
	void initialize();

	void update(Reference<const CameraInstance> camera);

	void stack_projection(u32 index) const;

	void stack_lighting(u32 index) const;

private:
	ConstantBuffer<VpBuffers> vpBuffers;
	ConstantBuffer<LightingPathBuffer> lightingBuffer;
};

}
