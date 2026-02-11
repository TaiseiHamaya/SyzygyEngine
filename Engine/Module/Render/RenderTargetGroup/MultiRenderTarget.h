#pragma once

#include "Engine/Module/Render/RenderTargetGroup/BaseRenderTargetGroup.h"

#include <array>

#include <d3d12.h>

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/DepthStencilTexture.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/RenderTexture.h"

/// <summary>
/// マルチレンダーターゲット
/// </summary>
/// <typeparam name="NumRenderTarget">レンダーターゲット数</typeparam>
namespace szg {

template<u32 NumRenderTarget = 2>
class MultiRenderTarget final : public BaseRenderTargetGroup {
	static_assert(NumRenderTarget >= 2, "NumRenderTargetは2以上である必要があります");
public:
	MultiRenderTarget() = default;
	~MultiRenderTarget() noexcept = default;

	SZG_CLASS_MOVE_ONLY(MultiRenderTarget)

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="renderTextures_"></param>
	void initialize(std::array<Reference<RenderTexture>, NumRenderTarget> renderTextures_);

private:
	void start_render_target(Reference<DepthStencilTexture> depthStencil) override;

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void clear_render_target() override;

private:
	std::array<Reference<RenderTexture>, NumRenderTarget> renderTextures;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, NumRenderTarget> renderTargetsHandles;
};

template<u32 NumRenderTarget>
inline void MultiRenderTarget<NumRenderTarget>::initialize(std::array<Reference<RenderTexture>, NumRenderTarget> renderTextures_) {
	renderTextures = renderTextures_;
	for (u32 i = 0; i < NumRenderTarget; ++i) {
		renderTargetsHandles[i] = renderTextures[i]->get_as_rtv()->handle();
	}
}

template<u32 NumRenderTarget>
inline void MultiRenderTarget<NumRenderTarget>::start_render_target(Reference<DepthStencilTexture> depthStencil) {
	for (Reference<RenderTexture>& renderTarget : renderTextures) {
		renderTarget->start_write();
	}
	auto&& commandList = DxCommand::GetCommandList();
	commandList->OMSetRenderTargets(
		static_cast<UINT>(renderTargetsHandles.size()), renderTargetsHandles.data(),
		false,
		depthStencil ? &depthStencil->get_as_dsv()->handle() : nullptr
	);
}

template<u32 NumRenderTarget>
inline void MultiRenderTarget<NumRenderTarget>::clear_render_target() {
	for (Reference<RenderTexture>& renderTarget : renderTextures) {
		renderTarget->get_as_rtv()->clear();
	}
}

}; // szg
