#pragma once

#include "Engine/Module/Render/RenderTargetGroup/BaseRenderTargetGroup.h"

#include <vector>

#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/ScreenTexture.h"
#include "Engine/GraphicsAPI/RenderingSystemValues.h"

namespace szg {

class SwapChainRenderTargetGroup final : public BaseRenderTargetGroup {
public:
	SwapChainRenderTargetGroup() = default;
	~SwapChainRenderTargetGroup() = default;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void initialize();

	/// <summary>
	/// resourceの設定
	/// </summary>
	/// <param name="resource_">設定するResource</param>
	/// <param name="index">SwapChainIndex</param>
	void set_resource(Reference<ScreenTexture>, u32 index);

public:
	void set_clear_color(const ColorRGBA& color) noexcept;

private:
	void start_render_target(Reference<DepthStencilTexture> depthStencil) override;

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void clear_render_target() override;

private:
	ColorRGBA clearColor{ CColorRGBA::BLACK };
	std::vector<Reference<ScreenTexture>> textures;
};

}; // szg
