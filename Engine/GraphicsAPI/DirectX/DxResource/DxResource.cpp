#include "DxResource.h"

#include "Engine/Application/Logger.h"
#include "Engine/GraphicsAPI/DirectX/DxDevice/DxDevice.h"

using namespace szg;

Microsoft::WRL::ComPtr<ID3D12Resource>& DxResource::get_resource() noexcept {
	return resource;
}

const Microsoft::WRL::ComPtr<ID3D12Resource>& DxResource::get_resource() const noexcept {
	return resource;
}

[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> DxResource::CreateBufferResource(size_t sizeInByte) {
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource;
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadのHeap
	D3D12_RESOURCE_DESC bufferResourceDesc{}; // 頂点リソースの設定
	bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; // バッファリソース
	bufferResourceDesc.Width = sizeInByte; // リソースのサイズ、引数をそのまま転送
	bufferResourceDesc.Height = 1; // バッファの場合はこれ
	bufferResourceDesc.DepthOrArraySize = 1;
	bufferResourceDesc.MipLevels = 1;
	bufferResourceDesc.SampleDesc.Count = 1;
	bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr;
	// 生成
	hr = DxDevice::GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &bufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
	szgErrorIf(FAILED(hr), "Failed creating GPU buffer. Size-\'{}\'", sizeInByte);
	return bufferResource;
}
