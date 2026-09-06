#include "ParticleBillboardDrawExecutor.h"

#include "Engine/Assets/PrimitiveGeometry/PrimitiveGeometryAsset.h"
#include "Engine/Assets/PrimitiveGeometry/PrimitiveGeometryLibrary.h"
#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"

using namespace szg;

void ParticleBillboardDrawExecutor::reinitialize(BlendMode type_, u32 maxInstance_) {
	type = type_;
	maxInstance = maxInstance_;
	matrices.initialize(maxInstance);
	rectData.initialize(maxInstance);
	material.initialize(maxInstance);
}

void ParticleBillboardDrawExecutor::draw_command() const {
	if (instanceCounter == 0) {
		return;
	}

	auto& commandList = DxCommand::GetCommandList();
	commandList->IASetVertexBuffers(0, 0, nullptr);
	auto geometry = PrimitiveGeometryLibrary::GetPrimitiveGeometry("Rect3D");
	commandList->IASetIndexBuffer(geometry->get_p_ibv());
	commandList->SetGraphicsRootDescriptorTable(0, matrices.get_handle_gpu());
	commandList->SetGraphicsRootDescriptorTable(1, rectData.get_handle_gpu());
	commandList->SetGraphicsRootDescriptorTable(2, material.get_handle_gpu());

	commandList->DrawIndexedInstanced(6, instanceCounter, 0, 0, 0);
}

void ParticleBillboardDrawExecutor::write_to_buffer(Reference<const ParticleDrawItem> instance) {
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
	rectData[next] = ParticleBillboardDataBuffer{
		.size = instance->size,
		.pivot = instance->pivot,
		.angle = instance->billboardAngle,
		.padding = 0.0f,
	};
	material[next] = MaterialDataBufferRGBA{
		.color = instance->color,
		.lighting = LighingType::None,
		.shininess = 50,
		.textureIndex = instance->textureIndex,
		.uvTransformMatrix = instance->uvTransform,
	};
}
