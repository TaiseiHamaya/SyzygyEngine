#include "CameraInstance.h"

using namespace szg;

void szg::CameraInstance::setup(std::unique_ptr<ICameraProjection> projection_) noexcept {
	projection = std::move(projection_);

	update_affine();
}

void CameraInstance::update_affine() {
	if (!is_active()) {
		return;
	}
	// カメラそのもののMatrix更新
	WorldInstance::update_affine();

	// カメラ位置をもとにViewMatrixを更新
	make_view_matrix();

	if (projection) {
		projectionMatrix = projection->generate_matrix();
	}
	else {
		projectionMatrix = CMatrix4x4::IDENTITY;
	}

	vpMatrix = viewAffine.to_matrix() * projectionMatrix;
}

const Affine& CameraInstance::view_affine() const {
	return viewAffine;
}

const Matrix4x4& CameraInstance::proj_matrix() const {
	return projectionMatrix;
}

const Matrix4x4& szg::CameraInstance::vp_matrix() const {
	return vpMatrix;
}

void CameraInstance::make_view_matrix() {
	viewAffine = world_affine().inverse_fast();
}

Matrix4x4 CameraInstance::MakeViewportMatrix(const Vector2& origin, const Vector2& size, r32 minDepth, r32 maxDepth) {
	return {
		{{ size.x / 2, 0, 0, 0 },
		{ 0, -size.y / 2, 0, 0 },
		{ 0, 0, maxDepth - minDepth, 0 },
		{ origin.x + size.x / 2, origin.y + size.y / 2, minDepth, 1 } }
	};
}
