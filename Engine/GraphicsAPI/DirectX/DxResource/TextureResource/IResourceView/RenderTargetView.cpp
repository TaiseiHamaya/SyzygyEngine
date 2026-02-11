#include "RenderTargetView.h"

using namespace szg;

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxDescriptorHeap/RTVDescriptorHeap/RTVDescriptorHeap.h"
#include "Engine/GraphicsAPI/DirectX/DxDevice/DxDevice.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/ITextureResource.h"

void RenderTargetView::release() {
	if (index.has_value()) {
		RTVDescriptorHeap::ReleaseIndex(index.value());
		index = std::nullopt;
	}
}

void RenderTargetView::create(Reference<ITextureResource> resource, DXGI_FORMAT format) {
	// ----------rtvの設定----------
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//resource->GetDesc().Format == DXGI_FORMAT_R8G8B8A8_UNORM ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : resource->GetDesc().Format;
	rtvDesc.Format = format; // 出力結果をSRGBに変換して書き込み
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして書き込む
	// 空いているアドレスをDescriptorHeapに問い合わせ
	index = RTVDescriptorHeap::UseHeapIndex();
	_handle = RTVDescriptorHeap::GetCPUHandle(index.value());
	// 生成
	DxDevice::GetDevice()->CreateRenderTargetView(resource->get_resource().Get(), &rtvDesc, _handle);
}

void RenderTargetView::clear(const ColorRGBA& color) const {
	auto&& commandList = DxCommand::GetCommandList();
	commandList->ClearRenderTargetView(
		_handle,
		&color.red, 0, nullptr
	);
}
