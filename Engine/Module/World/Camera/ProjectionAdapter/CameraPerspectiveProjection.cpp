#include "CameraPerspectiveProjection.h"

#include <cmath>
#include <cstdlib>

#include "Engine/Application/Logger.h"

void CameraPerspectiveProjection::initialize(r32 fovY_, r32 aspectRatio_, r32 nearClip_, r32 farClip_) noexcept {
	fovY = fovY_;
	aspectRatio = aspectRatio_;
	nearClip = nearClip_;
	farClip = farClip_;

	if (std::abs(farClip - nearClip) < 1e-6f) {
		szgWarning("Camera NearClip and FarClip are too close.");
	}
}

Matrix4x4 CameraPerspectiveProjection::generate_matrix() const noexcept {
	r32 cot = 1 / std::tan(fovY / 2);
	return Matrix4x4{
		{{ cot / aspectRatio, 0, 0, 0 },
		{ 0, cot, 0, 0 },
		{ 0, 0, farClip / (farClip - nearClip), 1 },
		{ 0, 0, -nearClip * farClip / (farClip - nearClip), 0 } }
	};
}
