#pragma once

#include <memory>
#include <vector>

#include <dxgi1_5.h>
#include <wrl/client.h>

#include <Library/Math/ColorRGBA.h>
#include <Library/Utility/Template/Reference.h>

namespace szg {

class SwapChainRenderTargetGroup;
class PSOBuilder;
class ScreenTexture;

class DxSwapChain final {
private:
	DxSwapChain() noexcept = default;

public:
	~DxSwapChain() noexcept = default;

private:
	DxSwapChain(const DxSwapChain&) = delete;
	DxSwapChain& operator=(const DxSwapChain&) = delete;

public:
	static void Initialize();
	static void Finalize();

public:
	static void SwapScreen();

public:
	static Reference<SwapChainRenderTargetGroup> GetRenderTarget();
	static Reference<ScreenTexture> GetWriteBufferTexture();

	static void SetClearColor(const ColorRGBA& color_) noexcept;
	static void EndRenderTarget();

private:
	static DxSwapChain& GetInstance() noexcept;

private:
	void create_swapchain();
	void create_render_target();
	void swap_screen();

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	std::vector<std::unique_ptr<ScreenTexture>> textures;
	std::unique_ptr<SwapChainRenderTargetGroup> renderTargetGroup;
};

}; // szg
