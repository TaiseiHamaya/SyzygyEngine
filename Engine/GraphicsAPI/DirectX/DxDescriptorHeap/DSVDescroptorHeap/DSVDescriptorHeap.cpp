#include "DSVDescriptorHeap.h"

#include "Engine/Application/Logger.h"
#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/GraphicsAPI/DirectX/DxDevice/DxDevice.h"

using namespace szg;

DSVDescriptorHeap& DSVDescriptorHeap::GetInstance() noexcept {
	static DSVDescriptorHeap instance{};
	return instance;
}

void DSVDescriptorHeap::Initialize() {
	GetInstance().create_descriptor_heap();
	GetInstance().initialize();
}

const u32 DSVDescriptorHeap::UseHeapIndex() noexcept {
	auto useIndex = GetInstance().use_heap_index();
	szgInformation("Use DSV index. Index-\'{}\'", useIndex);
	return useIndex;
}

const D3D12_CPU_DESCRIPTOR_HANDLE DSVDescriptorHeap::GetCPUHandle(u32 index) noexcept {
	return GetInstance().get_cpu_handle(index);
}

void DSVDescriptorHeap::ReleaseHeapIndex(u32 index) {
	szgInformation("Release DSV index. Index-\'{}\'", index);
	GetInstance().release_heap(index);
}

void DSVDescriptorHeap::create_descriptor_heap() {
	descriptorHeap = DxDescriptorHeap::CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		ProjectSettings::GetGraphicsSettingsImm().dsvHeapSize,
		false
	);
}

void DSVDescriptorHeap::initialize() {
	heapStartCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// DSVはGPUHandle参照不可
	//heapStartGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	incrementSize = DxDevice::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	releasedHeap.clear();
	nowHeapIndex = 0;
}
