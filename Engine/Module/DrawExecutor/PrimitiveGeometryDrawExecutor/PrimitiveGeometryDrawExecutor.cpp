#ifdef DEBUG_FEATURES_ENABLE

#include "PrimitiveGeometryDrawExecutor.h"

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"

using namespace szg;

PrimitiveGeometryDrawExecutor::PrimitiveGeometryDrawExecutor(std::shared_ptr<const PrimitiveGeometryAsset> asset_, u32 maxInstance) {
	reinitialize(asset_, maxInstance);
}

void PrimitiveGeometryDrawExecutor::reinitialize(std::shared_ptr<const PrimitiveGeometryAsset> asset_, u32 maxInstance_) {
	asset = asset_;
	maxInstance = maxInstance_;
	matrices.initialize(maxInstance);
}

void PrimitiveGeometryDrawExecutor::draw_command() const {
	if (!asset || instanceCounter == 0) {
		return;
	}

	auto& commandList = DxCommand::GetCommandList();
	commandList->IASetVertexBuffers(0, 1, &asset->get_vbv());
	commandList->IASetIndexBuffer(asset->get_p_ibv());
	commandList->SetGraphicsRootDescriptorTable(0, matrices.get_handle_gpu());

	commandList->DrawIndexedInstanced(asset->index_size(), instanceCounter, 0, 0, 0);
}

void PrimitiveGeometryDrawExecutor::write_to_buffer(Reference<const Matrix4x4> worldMatrix) {
	if (instanceCounter >= maxInstance) {
		return;
	}

	matrices[instanceCounter] = *worldMatrix;
	++instanceCounter;
}

#endif // _DEBUG
