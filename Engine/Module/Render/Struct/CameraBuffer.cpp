#include "CameraBuffer.h"

#include "Engine/Module/World/Camera/CameraInstance.h"

void szg::CameraBuffer::initialize() {
}

void szg::CameraBuffer::update(Reference<const CameraInstance> camera) {
	if (!camera) {
		return;
	}

	vpBuffers.data_mut()->view = camera->view_affine().to_matrix();
	vpBuffers.data_mut()->viewProjection = camera->vp_matrix_();
	lightingBuffer.data_mut()->viewInv = camera->view_affine().inverse_fast().to_matrix();
	lightingBuffer.data_mut()->position = camera->world_position();
	lightingBuffer.data_mut()->projInv = camera->proj_matrix().inverse();
}

void szg::CameraBuffer::stack_projection(u32 index) const {
	auto& commandList = DxCommand::GetCommandList();
	commandList->SetGraphicsRootConstantBufferView(
		index, vpBuffers.get_resource()->GetGPUVirtualAddress()
	);
}

void szg::CameraBuffer::stack_lighting(u32 index) const {
	auto& commandList = DxCommand::GetCommandList();
	commandList->SetGraphicsRootConstantBufferView(
		index, lightingBuffer.get_resource()->GetGPUVirtualAddress()
	);
}
