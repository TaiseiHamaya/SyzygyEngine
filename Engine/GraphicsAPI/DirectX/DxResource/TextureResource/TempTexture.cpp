#include "TempTexture.h"

#include "Engine/Application/Logger.h"
#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/GraphicsAPI/DirectX/DxDevice/DxDevice.h"

using namespace szg;

TempTexture::~TempTexture() {
	srv.release();
}

void TempTexture::initialize() {
	width = ProjectSettings::ClientWidth();
	height = ProjectSettings::ClientHeight();
	create_resource();

	srv.create(this, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

	set_name(std::format(L"TempTexture-SRV{}", srv.get_index().value()));
}

void TempTexture::start_copy_dest() {
	change_resource_state(D3D12_RESOURCE_STATE_COPY_DEST);
}

void TempTexture::start_read() {
	change_resource_state(srv.use_state());
}

void TempTexture::create_resource() {
	D3D12_HEAP_PROPERTIES heapProperties{};// 利用するHeapを設定
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // 通常設定
	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	HRESULT hr = DxDevice::GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&resource));

	szgErrorIf(FAILED(hr), "Failed to allocate GPU memory. Width-\'{}\', Height-\'{}\', Format-\'{}\'",
		width, height, (i32)DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

	state = D3D12_RESOURCE_STATE_GENERIC_READ;
}
