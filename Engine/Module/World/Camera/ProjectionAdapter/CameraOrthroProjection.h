#pragma once

#include "./ICameraProjection.h"

namespace szg {

class CameraOrthroProjection final : public ICameraProjection {
public:
	void initialize(r32 left, r32 right, r32 bottom, r32 top, r32 nearClip, r32 farClip) noexcept;

	Matrix4x4 generate_matrix() const noexcept override;

private:
	r32 left;
	r32 right;
	r32 bottom;
	r32 top;
	r32 nearClip;
	r32 farClip;
};

}; // namespace szg
