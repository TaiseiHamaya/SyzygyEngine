#include "DepthStencilView.h"

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxDescriptorHeap/DSVDescroptorHeap/DSVDescriptorHeap.h"
#include "Engine/GraphicsAPI/DirectX/DxDevice/DxDevice.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/ITextureResource.h"

using namespace szg;

void DepthStencilView::release() {
	if (index.has_value()) {
		DSVDescriptorHeap::ReleaseHeapIndex(index.value());
		index = std::nullopt;
	}
}

void DepthStencilView::create(Reference<ITextureResource> resource, DXGI_FORMAT format_) {
	format = format_;
	D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
	desc.Format = format;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 画面全体なので2D
	index = DSVDescriptorHeap::UseHeapIndex();
	_handle = DSVDescriptorHeap::GetCPUHandle(index.value());
	// viewの作成
	DxDevice::GetDevice()->CreateDepthStencilView(
		resource->get_resource().Get(),
		&desc,
		_handle
	);
}

void DepthStencilView::clear(r32 depth, u8 stencil) const {
	auto&& commandList = DxCommand::GetCommandList();
	commandList->ClearDepthStencilView(
		_handle, D3D12_CLEAR_FLAG_DEPTH, depth, stencil, 0, nullptr
	);
}
