#include "DxSwapChain.h"

using namespace szg;

#include <memory>

#include "Engine/Application/ArgumentParser.h"
#include "Engine/Application/Logger.h"
#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/Application/WinApp.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxDevice/DxDevice.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/ScreenTexture.h"
#include "Engine/GraphicsAPI/DirectX/DxSystemValues.h"
#include "Engine/Module/Render/RenderTargetGroup/SwapChainRenderTargetGroup.h"

void DxSwapChain::Initialize() {
	auto& instance = GetInstance();
	instance.renderTargetGroup = std::make_unique<SwapChainRenderTargetGroup>();
	instance.renderTargetGroup->initialize();
	instance.create_swapchain();
	instance.create_render_target();
	SetClearColor(ProjectSettings::GetGraphicsSettingsImm().clearColor);
}

void DxSwapChain::Finalize() {
	auto& instance = GetInstance();
	for (std::unique_ptr<ScreenTexture>& texture : instance.textures) {
		texture.reset();
	}
	instance.renderTargetGroup.reset();
	instance.swapChain.Reset();
}

void DxSwapChain::SwapScreen() {
	GetInstance().swap_screen();
}

Reference<SwapChainRenderTargetGroup> DxSwapChain::GetRenderTarget() {
	return GetInstance().renderTargetGroup;
}

Reference<ScreenTexture> DxSwapChain::GetWriteBufferTexture() {
	return GetInstance().textures[RenderingSystemValues::NowBackbufferIndex()];
}

void DxSwapChain::SetClearColor(const ColorRGBA& color_) noexcept {
	GetInstance().renderTargetGroup->set_clear_color(color_);
}

void DxSwapChain::EndRenderTarget() {
	auto& instance = GetInstance();
	instance.textures[RenderingSystemValues::NowBackbufferIndex()]->start_present();
}

DxSwapChain& DxSwapChain::GetInstance() noexcept {
	static DxSwapChain instance{};
	return instance;
}

void DxSwapChain::create_swapchain() {
	HRESULT hr;
	// ----------スワップチェインを生成----------
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = ProjectSettings::ClientWidth(); // 画面の幅
	swapChainDesc.Height = ProjectSettings::ClientHeight(); // 画面の高さ
	swapChainDesc.Format = DxSystemValues::SWAPCHAIN_FORMAT; // 色の形式
	swapChainDesc.SampleDesc.Count = 1; // マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 画面のターゲットとして利用
	swapChainDesc.BufferCount = ProjectSettings::GetGraphicsSettingsImm().numBuffering; // ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニタに映したら、中身を破棄
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
#ifdef DEBUG_FEATURES_ENABLE
	swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING; // 限界までフレームレートを出せるようにする
#endif // _DEBUG

	// コマンドキュー、ウィンドウハンドル、設定を渡してスワップチェインを生成
	hr = DxDevice::GetFactory()->CreateSwapChainForHwnd(DxCommand::GetCommandQueue().Get(), WinApp::GetWndHandle(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
	// 失敗したら停止させる
	szgCriticalIf(FAILED(hr), "Failed creating swap chain.");
}

void DxSwapChain::create_render_target() {
	textures.resize(ProjectSettings::GetGraphicsSettingsImm().numBuffering);
	HRESULT hr;
	// RTVにリソースを生成
	// ダブルバッファなのでリソースを2つ作る
	for (u32 i = 0; i < ProjectSettings::GetGraphicsSettingsImm().numBuffering; ++i) {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(resource.GetAddressOf()));
		szgCriticalIf(FAILED(hr), "Failed creating swapchain render targets.");
		// テクスチャとして落とし込み
		textures[i] = std::make_unique<ScreenTexture>();
		textures[i]->initialize(resource);
		// RTGroupとして登録
		renderTargetGroup->set_resource(textures[i], i);
	}
}

void DxSwapChain::swap_screen() {
#ifdef DEBUG_FEATURES_ENABLE
	if (ArgumentParser::Contains("-UnlimitedFPS")) {
		swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
	}
	else {
		swapChain->Present(1, 0);
	}
#else
	swapChain->Present(1, 0);
#endif // _DEBUG
	RenderingSystemValues::ChangeBackbufferIndex(swapChain->GetCurrentBackBufferIndex());
}
