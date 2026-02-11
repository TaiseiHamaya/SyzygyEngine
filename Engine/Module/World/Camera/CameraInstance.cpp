#include "CameraInstance.h"

using namespace szg;

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"

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
}

void CameraInstance::transfer() {
	vpBuffers.data_mut()->view = viewAffine.to_matrix();
	vpBuffers.data_mut()->viewProjection = viewAffine.to_matrix() * projectionMatrix;
	lightingBuffer.data_mut()->viewInv = viewAffine.inverse_fast().to_matrix();
	lightingBuffer.data_mut()->position = world_position();
	lightingBuffer.data_mut()->projInv = projectionMatrix.inverse();
}

void CameraInstance::register_world_projection(u32 index) const {
	auto& commandList = DxCommand::GetCommandList();
	commandList->SetGraphicsRootConstantBufferView(
		index, vpBuffers.get_resource()->GetGPUVirtualAddress()
	);
}

void CameraInstance::register_world_lighting(u32 index) const {
	auto& commandList = DxCommand::GetCommandList();
	commandList->SetGraphicsRootConstantBufferView(
		index, lightingBuffer.get_resource()->GetGPUVirtualAddress()
	);
}

const Matrix4x4& CameraInstance::vp_matrix() const {
	return vpBuffers.data_imm()->viewProjection;
}

const Affine& CameraInstance::view_affine() const {
	return viewAffine;
}

const Matrix4x4& CameraInstance::proj_matrix() const {
	return projectionMatrix;
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
