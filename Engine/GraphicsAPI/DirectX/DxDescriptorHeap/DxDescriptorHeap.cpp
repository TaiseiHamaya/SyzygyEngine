#include "DxDescriptorHeap.h"

#include "Engine/Application/Logger.h"
#include "Engine/GraphicsAPI/DirectX/DxDevice/DxDevice.h"

using namespace szg;

const D3D12_CPU_DESCRIPTOR_HANDLE DxDescriptorHeap::get_cpu_handle(u32 index) const noexcept {
	D3D12_CPU_DESCRIPTOR_HANDLE result = heapStartCPU; // スタートから
	result.ptr += static_cast<SIZE_T>(incrementSize * index); // increment * index分ポインタを進める
	return result;
}

const D3D12_GPU_DESCRIPTOR_HANDLE DxDescriptorHeap::get_gpu_handle(u32 index) const noexcept {
	D3D12_GPU_DESCRIPTOR_HANDLE result = heapStartGPU; // 上に同じ
	result.ptr += incrementSize * static_cast<UINT64>(index);
	return result;
}

const u32 DxDescriptorHeap::use_heap_index() noexcept {
	if (releasedHeap.empty()) {
		return nowHeapIndex++;
	}
	else {
		u32 next = releasedHeap.front();
		releasedHeap.pop_front();
		return next;
	}
}

void DxDescriptorHeap::release_heap(u32 index) {
	releasedHeap.emplace_back(index);
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DxDescriptorHeap::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT heapSize, bool shaderVisible) {
	// カウンターを正常に保つためにComPtrを使用
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	// 引数を元に設定
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = heapSize;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr;
	// ここで生成
	hr = DxDevice::GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));
	szgErrorIf(FAILED(hr), "Failed creating descriptor heap. Type-\'{}\' Size-\'{}\'", (i32)heapType, heapSize);
	return descriptorHeap;
}
