#include "DepthStencilTexture.h"

#include <format>

#include "Engine/Application/Logger.h"
#include "Engine/GraphicsAPI/DirectX/DxDevice/DxDevice.h"

using namespace szg;

DepthStencilTexture::~DepthStencilTexture() {
	srv.release();
	dsv.release();
}

void DepthStencilTexture::initialize(DXGI_FORMAT format, u32 width_, u32 height_) {
	width = width_;
	height = height_;
	create_resource(format);
	dsv.create(this, format);
	srv.create(this, static_cast<DXGI_FORMAT>(format + 1));

	state = D3D12_RESOURCE_STATE_GENERIC_READ;

	set_name(std::format(L"DepthStencil-DSV{}, SRV{}", dsv.get_index().value(), srv.get_index().value()));
}

void DepthStencilTexture::start_write() {
	change_resource_state(dsv.use_state());
}

void DepthStencilTexture::start_read() {
	change_resource_state(srv.use_state());
}

void DepthStencilTexture::create_resource(DXGI_FORMAT format) {
	HRESULT hr;
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 画面全体なので2D
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DSとして使用

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; // 1.0fで初期化
	depthClearValue.Format = resourceDesc.Format;

	hr = DxDevice::GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&depthClearValue,
		IID_PPV_ARGS(resource.GetAddressOf())
	);
	szgErrorIf(FAILED(hr), "Failed create depth stencil. Size-\'[{}, {}]\', Format-\'{}\'", width, height, (i32)format);

}
