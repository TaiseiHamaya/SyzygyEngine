#include "ShaderResourceView.h"

#include "Engine/Application/Logger.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxDescriptorHeap/SRVDescriptorHeap/SRVDescriptorHeap.h"
#include "Engine/GraphicsAPI/DirectX/DxDevice/DxDevice.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/ITextureResource.h"

using namespace szg;

void ShaderResourceView::release() {
	if (index.has_value()) {
		SRVDescriptorHeap::ReleaseHeapIndex(index.value());
		index = std::nullopt;
	}
}

void ShaderResourceView::create(Reference<ITextureResource> resource, DXGI_FORMAT format) {
	auto& gpuResource = resource->get_resource();
	// 使用するディスクリプタヒープを取得
	index = SRVDescriptorHeap::UseHeapIndex();
	szgInformation("SRV-{}", index.value());
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = SRVDescriptorHeap::GetCPUHandle(index.value());
	_handle = SRVDescriptorHeap::GetGPUHandle(index.value());

	D3D12_RESOURCE_DESC desc = gpuResource->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = desc.MipLevels;

	// textureResourceに転送
	DxDevice::GetDevice()->CreateShaderResourceView(gpuResource.Get(), &srvDesc, handleCPU);
}

void ShaderResourceView::create_dds(Reference<ITextureResource> resource, DXGI_FORMAT format) {
	auto& gpuResource = resource->get_resource();
	// 使用するディスクリプタヒープを取得
	index = SRVDescriptorHeap::UseHeapIndex();
	szgInformation("SRV-{}", index.value());
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = SRVDescriptorHeap::GetCPUHandle(index.value());
	_handle = SRVDescriptorHeap::GetGPUHandle(index.value());

	D3D12_RESOURCE_DESC desc = gpuResource->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE; // 2Dテクスチャ
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = UINT_MAX;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

	// textureResourceに転送
	DxDevice::GetDevice()->CreateShaderResourceView(gpuResource.Get(), &srvDesc, handleCPU);
}

void ShaderResourceView::use(u32 paramIndex) const {
	auto&& commandList = DxCommand::GetCommandList();
	commandList->SetGraphicsRootDescriptorTable(paramIndex, _handle);
}
