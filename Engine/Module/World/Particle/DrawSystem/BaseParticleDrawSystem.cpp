#include "BaseParticleDrawSystem.h"

using namespace szg;

void BaseParticleDrawSystem::create_buffers(u32 bufferSize) {
	particleBuffer.initialize(bufferSize);
}

void BaseParticleDrawSystem::write_to_buffer(u32 index, const Matrix4x4& worldMatrix, const Matrix4x4& uvMatrix, const ColorRGBA& color) {
	if (index >= particleBuffer.size()) {
	}
	particleBuffer[index] = {
		worldMatrix,
		uvMatrix,
		color
	};
}
