#include "RenderTexture.h"

#include <format>

#include "Engine/Application/Logger.h"
#include "Engine/GraphicsAPI/DirectX/DxDevice/DxDevice.h"

using namespace szg;

RenderTexture::~RenderTexture() {
	srv.release();
	rtv.release();
}

void RenderTexture::initialize(DXGI_FORMAT format, u32 width_, u32 height_) {
	width = width_;
	height = height_;
	create_resource(format);
	rtv.create(this, format);
	srv.create(this, format);

	set_name(std::format(L"RenderTexture-RTV{}, SRV{}", rtv.get_index().value(), srv.get_index().value()));
}

void RenderTexture::start_write() {
	change_resource_state(rtv.use_state());
}

void RenderTexture::start_read() {
	change_resource_state(srv.use_state());
}

void RenderTexture::create_resource(DXGI_FORMAT format) {
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2Dテクスチャを作成
	desc.Alignment = 0;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // レンダーターゲットとして使用する

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.Color[3] = 1.0f;

	HRESULT hr;
	hr = DxDevice::GetDevice()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // 最初はRead状態から開始
		&clearValue,
		IID_PPV_ARGS(resource.GetAddressOf())
	);
	szgErrorIf(FAILED(hr), "Failed create offscreen resource. Width-\'{}\', Height-\'{}\', Format-\'{}\'",
		width, height, (i32)format);

	state = D3D12_RESOURCE_STATE_GENERIC_READ;
}
