#include "DirectionalLightingExecutor.h"

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/Module/World/Light/DirectionalLight/DirectionalLightInstance.h"

using namespace szg;

DirectionalLightingExecutor::DirectionalLightingExecutor(u32 maxInstance_) {
	reinitialize(maxInstance_);
}

void DirectionalLightingExecutor::reinitialize(u32 maxInstance_) {
	maxInstance = maxInstance_;
	lightData.initialize(maxInstance);
}

void DirectionalLightingExecutor::draw_command() const {
	if (instanceCounter == 0) {
		return;
	}

	auto&& command = DxCommand::GetCommandList();
	command->SetGraphicsRootDescriptorTable(0, lightData.get_handle_gpu());
	command->DrawInstanced(3, instanceCounter, 0, 0);
}

void DirectionalLightingExecutor::set_command(u32 paramIndex) const {
	auto&& command = DxCommand::GetCommandList();
	command->SetGraphicsRootDescriptorTable(paramIndex, lightData.get_handle_gpu());
}

void DirectionalLightingExecutor::write_to_buffer(Reference<const DirectionalLightInstance> instance) {
	if (!instance || !instance->is_active()) {
		return;
	}
	if (instanceCounter >= maxInstance) {
		return;
	}

	lightData[instanceCounter] = instance->light_data_imm();
	++instanceCounter;
}
