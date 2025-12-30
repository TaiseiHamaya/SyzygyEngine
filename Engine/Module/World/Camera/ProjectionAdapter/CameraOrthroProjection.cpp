#include "CameraOrthroProjection.h"

#include <cstdlib>

#include "Engine/Application/Logger.h"

void CameraOrthroProjection::initialize(r32 left_, r32 right_, r32 bottom_, r32 top_, r32 nearClip_, r32 farClip_) noexcept {
	left = left_;
	right = right_;
	bottom = bottom_;
	top = top_;
	nearClip = nearClip_;
	farClip = farClip_;

	if (std::abs(right - left) < 1e-6f) {
		szgWarning("Camera Left and Right are too close.");
	}
	if (std::abs(bottom - top) < 1e-6) {
		szgWarning("Camera Bottom and Top are too close.");
	}
	if (std::abs(farClip - nearClip) < 1e-6f) {
		szgWarning("Camera NearClip and FarClip are too close.");
	}
}

Matrix4x4 CameraOrthroProjection::generate_matrix() const noexcept {
	return Matrix4x4{
		{ { 2 / (right - left), 0, 0, 0},
		{ 0, 2 / (top - bottom), 0, 0},
		{ 0, 0, 1 / (farClip - nearClip), 0},
		{ (left + right) / (left - right), (bottom + top) / (bottom - top), nearClip / (nearClip - farClip), 1 } }
	};
}
