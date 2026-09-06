#include "ParticleMeshDrawExecutor.h"

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"

using namespace szg;

void ParticleMeshDrawExecutor::reinitialize(std::shared_ptr<const PolygonMesh> asset_, BlendMode type_, u32 maxInstance_) {
	asset = asset_;
	type = type_;
	maxInstance = maxInstance_;
	matrices.initialize(maxInstance);
	size_t materialCount = asset ? asset->material_count() : 1;
	materials.resize(materialCount);
	for (StructuredBuffer<MaterialDataBufferRGBA>& material : materials) {
		material.initialize(maxInstance);
	}
}

void ParticleMeshDrawExecutor::draw_command() const {
	if (!asset || instanceCounter == 0) {
		return;
	}

	auto& commandList = DxCommand::GetCommandList();
	for (u32 i = 0; i < asset->material_count(); ++i) {
		commandList->IASetVertexBuffers(0, 1, &asset->get_vbv(i));
		commandList->IASetIndexBuffer(asset->get_p_ibv(i));
		commandList->SetGraphicsRootDescriptorTable(0, matrices.get_handle_gpu());
		commandList->SetGraphicsRootDescriptorTable(1, materials[i].get_handle_gpu());

		commandList->DrawIndexedInstanced(asset->index_size(i), instanceCounter, 0, 0, 0);
	}
}

void ParticleMeshDrawExecutor::write_to_buffer(Reference<const ParticleDrawItem> instance) {
	if (!instance || !instance->is_draw()) {
		return;
	}

	u32 next;
	{
		std::lock_guard<std::mutex> lock{ writeBufferMutex };

		if (instanceCounter >= maxInstance) {
			return;
		}

		next = instanceCounter;
		++instanceCounter;
	}

	matrices[next] = {
		.world = instance->world,
		.itWorld = instance->world.inverse().get_basis().transposed()
	};
	for (u32 i = 0; i < materials.size(); ++i) {
		materials[i][next] = MaterialDataBufferRGBA{
			.color = instance->color,
			.lighting = LighingType::None,
			.shininess = 50,
			.textureIndex = instance->textureIndex,
			.uvTransformMatrix = instance->uvTransform,
		};
	}
}
