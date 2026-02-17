#include "ITextureResource.h"

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"

using namespace szg;

void ITextureResource::change_resource_state(D3D12_RESOURCE_STATES next) {
	if (state == next) {
		return;
	}
	DxCommand::SetBarrier(resource, state, next);
	state = next;
}

void ITextureResource::set_name(const std::wstring& name) {
	resource->SetName(name.c_str());
}
