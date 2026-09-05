#include "ScreenTexture.h"

#include "Engine/Application/EngineSettings.h"
#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/GraphicsAPI/DirectX/DxSystemValues.h"

using namespace szg;

ScreenTexture::~ScreenTexture() {
	rtv.release();
}

void ScreenTexture::initialize(Microsoft::WRL::ComPtr<ID3D12Resource> resource_) {
	resource = resource_;
	width = ProjectSettings::ClientWidth();
	height = ProjectSettings::ClientHeight();
	rtv.create(this, DxSystemValues::SCREEN_RTV_FORMAT);

	state = D3D12_RESOURCE_STATE_PRESENT;

	set_name(std::format(L"ScreenTexture-RTV{}", rtv.get_index().value()));
}

void ScreenTexture::start_write() {
	change_resource_state(rtv.use_state());
}

void ScreenTexture::start_present() {
	change_resource_state(D3D12_RESOURCE_STATE_PRESENT);
}

#ifdef DEBUG_FEATURES_ENABLE
void ScreenTexture::start_read() {
	change_resource_state(D3D12_RESOURCE_STATE_GENERIC_READ);
}
#endif // DEBUG_FEATURES_ENABLE
