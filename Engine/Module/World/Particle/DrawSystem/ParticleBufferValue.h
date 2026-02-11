#pragma once

#include <Library/Math/ColorRGBA.h>
#include <Library/Math/Matrix4x4.h>

namespace szg {

struct ParticleBuffer {
	Matrix4x4 world{ CMatrix4x4::IDENTITY };
	Matrix4x4 uvMatrix{ CMatrix4x4::IDENTITY };
	ColorRGBA color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

}; // szg
