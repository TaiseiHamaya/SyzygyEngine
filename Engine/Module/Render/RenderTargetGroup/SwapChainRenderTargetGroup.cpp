#include "SwapChainRenderTargetGroup.h"

using namespace szg;

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"

void SwapChainRenderTargetGroup::initialize() {
	textures.resize(ProjectSettings::GetGraphicsSettingsImm().numBuffering);
}

void SwapChainRenderTargetGroup::set_resource(Reference<ScreenTexture> texture, u32 index) {
	textures[index] = texture;
}

void SwapChainRenderTargetGroup::set_clear_color(const ColorRGBA& color) noexcept {
	clearColor = color;
}

void SwapChainRenderTargetGroup::start_render_target(Reference<DepthStencilTexture> depthStencil) {
	u32 backBufferIndex = RenderingSystemValues::NowBackbufferIndex();
	textures[backBufferIndex]->start_write();
	auto&& commandList = DxCommand::GetCommandList();
	commandList->OMSetRenderTargets(
		1, &textures[backBufferIndex]->get_as_rtv()->handle(),
		depthStencil ? 1 : 0,
		depthStencil ? &depthStencil->get_as_dsv()->handle() : nullptr
	);
}

void SwapChainRenderTargetGroup::clear_render_target() {
	u32 backBufferIndex = RenderingSystemValues::NowBackbufferIndex();
	textures[backBufferIndex]->get_as_rtv()->clear(clearColor);
}
