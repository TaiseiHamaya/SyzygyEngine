#include "DxCommand.h"

#include "Engine/Application/Logger.h"
#include "Engine/GraphicsAPI/DirectX/DxDevice/DxDevice.h"

#include <d3dx12.h>

using namespace szg;

void DxCommand::Initialize() {
	GetInstance();
	GetInstance().create_command();
	GetInstance().create_fence();
}

const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>& DxCommand::GetCommandList() noexcept {
	return GetInstance().commandList;
}

const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& DxCommand::GetCommandQueue() noexcept {
	return GetInstance().commandQueue;
}

void DxCommand::SetBarrier(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource.Get();
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	DxCommand::GetCommandList()->ResourceBarrier(1, &barrier); // バリアの適用
}

// ----------------------ここから後で直す----------------------
void DxCommand::SetTextureCommand(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, const Microsoft::WRL::ComPtr<ID3D12Resource>& intermediateResource, const std::vector<D3D12_SUBRESOURCE_DATA>& subResources) {
	UpdateSubresources(GetInstance().commandListTexture.Get(), resource.Get(), intermediateResource.Get(), 0, 0, UINT(subResources.size()), subResources.data()); // コマンドに積む

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	GetInstance().commandListTexture->ResourceBarrier(1, &barrier);
}

void DxCommand::ExecuteTextureCommand() {
	HRESULT hr;
	ID3D12GraphicsCommandList* commandList = GetInstance().commandListTexture.Get();
	// コマンドリストのクローズ
	hr = commandList->Close();
	szgErrorIf(FAILED(hr), "");

	// まとめる
	ID3D12CommandList* commandLists[] = { commandList };
	// キック
	GetInstance().commandQueueTexture->ExecuteCommandLists(_countof(commandLists), commandLists);
}

void DxCommand::WaitTextureCommand() {
	// ----------シグナルの発行----------
	const u64& fenceIndex = ++GetInstance().textureFenceIndex;
	Microsoft::WRL::ComPtr<ID3D12Fence>& fence = GetInstance().textureFence;
	HANDLE& fenceEvent = GetInstance().textureFenceEvent;
	GetInstance().commandQueueTexture->Signal(fence.Get(), fenceIndex);
	// ----------シグナルまで到達してるか----------
	if (fence->GetCompletedValue() < fenceIndex) {
		fence->SetEventOnCompletion(fenceIndex, fenceEvent);
		// 終わるまで待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void DxCommand::ResetTextureCommand() {
	HRESULT hr;
	// ----------リセット----------
	hr = GetInstance().commandAllocatorTexture->Reset();
	szgErrorIf(FAILED(hr), "");

	hr = GetInstance().commandListTexture->Reset(GetInstance().commandAllocatorTexture.Get(), nullptr);
	szgErrorIf(FAILED(hr), "");
}
// ----------------------ここまで後で直す----------------------

DxCommand& DxCommand::GetInstance() noexcept {
	static DxCommand instance{};
	return instance;
}

void DxCommand::create_command() {
	const Microsoft::WRL::ComPtr<ID3D12Device>& device = DxDevice::GetDevice();
	HRESULT hr;
	// ----------コマンドキューの生成----------
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue.GetAddressOf()));
	szgCriticalIf(FAILED(hr), "Failed create command queue.");

	// ----------コマンドアロケータの生成----------
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf()));
	szgCriticalIf(FAILED(hr), "Failed create command allocator.");

	// ----------コマンドリストを生成する----------
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf()));
	szgCriticalIf(FAILED(hr), "Failed create command list.");

	// ----------コマンドキューの生成----------
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueueTexture.GetAddressOf()));
	szgCriticalIf(FAILED(hr), "Failed create command queue.");

	// ----------テクスチャ用コマンドアロケータの生成----------
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocatorTexture));
	szgCriticalIf(FAILED(hr), "Failed create command allocator");

	// ----------テクスチャ用コマンドリストを生成する----------
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocatorTexture.Get(), nullptr, IID_PPV_ARGS(commandListTexture.GetAddressOf()));
	szgCriticalIf(FAILED(hr), "Failed create command list.");

}

void DxCommand::create_fence() {
	HRESULT hr;
	// ----------フェンスの生成----------
	u64 fenceValue = 0;
	hr = DxDevice::GetDevice()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	szgCriticalIf(FAILED(hr), "");

	hr = DxDevice::GetDevice()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&textureFence));
	szgCriticalIf(FAILED(hr), "");

	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	szgCriticalIf(fenceEvent == nullptr, "Failed create fence event.");

	textureFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	szgCriticalIf(textureFenceEvent == nullptr, "Failed create fence event.");
	fenceIndex = 0;
	textureFenceIndex = 0;
}

void DxCommand::close_and_kick() {
	HRESULT hr;
	// コマンドリストのクローズ
	hr = commandList->Close();
	szgErrorIf(FAILED(hr), "Failed close GPU command.");
	// まとめる
	ID3D12CommandList* commandLists[] = { commandList.Get() };
	// キック
	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

void DxCommand::wait_command() {
	// ----------シグナルの発行----------
	++fenceIndex;
	commandQueue->Signal(fence.Get(), fenceIndex);
	// ----------シグナルまで到達してるか----------
	if (fence->GetCompletedValue() < fenceIndex) {
		fence->SetEventOnCompletion(fenceIndex, fenceEvent);
		// 終わるまで待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void DxCommand::reset() {
	HRESULT hr;
	// ----------リセット----------
	hr = commandAllocator->Reset();
	szgErrorIf(FAILED(hr), "Failed reset command allocator.");

	hr = commandList->Reset(commandAllocator.Get(), nullptr);
	szgErrorIf(FAILED(hr), "Failed reset command list.");
}
