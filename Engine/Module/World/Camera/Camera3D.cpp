#include "Camera3D.h"

using namespace szg;

#include <cmath>

#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"

void Camera3D::initialize() {
	set_perspective_fov_info(
		0.45f,
		(float)ProjectSettings::ClientWidth() / ProjectSettings::ClientHeight(),
		0.1f, 1000
	);

	update_affine();
}

void Camera3D::update_affine() {
	if (!is_active()) {
		return;
	}
	// カメラそのもののMatrix更新
	WorldInstance::update_affine();

	// カメラ位置をもとにViewMatrixを更新
	make_view_matrix();
	make_perspective_matrix();
}

void Camera3D::transfer() {
	vpBuffers.data_mut()->view = viewAffine.to_matrix();
	vpBuffers.data_mut()->viewProjection = viewAffine.to_matrix() * projectionMatrix;
	lightingBuffer.data_mut()->viewInv = viewAffine.inverse_fast().to_matrix();
	lightingBuffer.data_mut()->position = world_position();
	lightingBuffer.data_mut()->projInv = projectionMatrix.inverse();
}

void Camera3D::register_world_projection(u32 index) const {
	auto& commandList = DxCommand::GetCommandList();
	commandList->SetGraphicsRootConstantBufferView(
		index, vpBuffers.get_resource()->GetGPUVirtualAddress()
	);
}

void Camera3D::register_world_lighting(u32 index) const {
	auto& commandList = DxCommand::GetCommandList();
	commandList->SetGraphicsRootConstantBufferView(
		index, lightingBuffer.get_resource()->GetGPUVirtualAddress()
	);
}

void Camera3D::set_transform(const Transform3D& transform_) noexcept {
	transform.copy(transform_);
}

void Camera3D::set_perspective_fov_info(r32 fovY_, r32 aspectRatio_, r32 nearClip_, r32 farClip_) noexcept {
	fovY = fovY_;
	aspectRatio = aspectRatio_;
	nearClip = nearClip_;
	farClip = farClip_;
}

const Matrix4x4& Camera3D::vp_matrix() const {
	return vpBuffers.data_imm()->viewProjection;
}

const Affine& Camera3D::view_affine() const {
	return viewAffine;
}

const Matrix4x4& Camera3D::proj_matrix() const {
	return projectionMatrix;
}

void Camera3D::make_view_matrix() {
	viewAffine = world_affine().inverse_fast();
}

void Camera3D::make_perspective_matrix() {
	r32 cot = 1 / std::tan(fovY / 2);
	projectionMatrix = {
		{{ cot / aspectRatio, 0, 0, 0 },
		{ 0, cot, 0, 0 },
		{ 0, 0, farClip / (farClip - nearClip), 1 },
		{ 0, 0, -nearClip * farClip / (farClip - nearClip), 0 } }
	};
}

Matrix4x4 Camera3D::MakeViewportMatrix(const Vector2& origin, const Vector2& size, r32 minDepth, r32 maxDepth) {
	return {
		{{ size.x / 2, 0, 0, 0 },
		{ 0, -size.y / 2, 0, 0 },
		{ 0, 0, maxDepth - minDepth, 0 },
		{ origin.x + size.x / 2, origin.y + size.y / 2, minDepth, 1 } }
	};
}
