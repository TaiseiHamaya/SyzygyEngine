#pragma once

#include <Library/Math/Vector2.h>
#include <Library/Math/Vector3.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/GraphicsAPI/DirectX/DxResource/ConstantBuffer/ConstantBuffer.h"

namespace szg {

class EditorWorldGridBuffer final {
public:
	struct Data {
		Vector2 center;
		r32 scale;
		u32 padding;
	};

public:
	EditorWorldGridBuffer() noexcept = default;
	~EditorWorldGridBuffer() noexcept = default;

	SZG_CLASS_MOVE_ONLY(EditorWorldGridBuffer)

public:
	void transfer(const Vector3& centerBase, r32 cameraY);

	void stack_command() const;

private:
	szg::ConstantBuffer<Data> data;
};

}; // szg
