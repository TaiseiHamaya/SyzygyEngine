#include "RTVDescriptorHeap.h"

#include "Engine/Application/Logger.h"
#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/GraphicsAPI/DirectX/DxDevice/DxDevice.h"

using namespace szg;

RTVDescriptorHeap& RTVDescriptorHeap::GetInstance() noexcept {
	static RTVDescriptorHeap instance{};
	return instance;
}

void RTVDescriptorHeap::Initialize() {
	GetInstance().create_descriptor_heap();
	GetInstance().initialize();
}

u32 RTVDescriptorHeap::UseHeapIndex() noexcept {
	auto useIndex = GetInstance().use_heap_index();
	szgInformation("Use RTV index. Index-\'{}\'", useIndex);
	return useIndex;
}

D3D12_CPU_DESCRIPTOR_HANDLE RTVDescriptorHeap::GetCPUHandle(u32 index) noexcept {
	return GetInstance().get_cpu_handle(index);
}

void RTVDescriptorHeap::ReleaseIndex(u32 index) {
	szgInformation("Release RTV index. Index-\'{}\'", index);
	GetInstance().release_heap(index);
}

void RTVDescriptorHeap::create_descriptor_heap() {
	descriptorHeap =
		DxDescriptorHeap::CreateDescriptorHeap(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			ProjectSettings::GetGraphicsSettingsImm().rtvHeapSize,
			false
		);
}

void RTVDescriptorHeap::initialize() {
	heapStartCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// RTVはGPUHandle参照不可
	//heapStartGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	incrementSize = DxDevice::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	releasedHeap.clear();
	nowHeapIndex = 0;
}
