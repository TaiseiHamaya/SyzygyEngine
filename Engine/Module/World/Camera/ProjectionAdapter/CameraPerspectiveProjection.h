#pragma once

#include "./ICameraProjection.h"

class CameraPerspectiveProjection final : public ICameraProjection {
public:
	void initialize(r32 fovY_, r32 aspectRatio_, r32 nearClip_, r32 farClip_) noexcept;

	Matrix4x4 generate_matrix() const noexcept override;

private:
	r32 fovY;
	r32 aspectRatio;
	r32 nearClip;
	r32 farClip;
};

