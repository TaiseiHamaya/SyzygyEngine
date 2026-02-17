#include "SingleRenderTarget.h"

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/DepthStencilTexture.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/RenderTexture.h"

using namespace szg;

void SingleRenderTarget::initialize(Reference<RenderTexture> texture_) {
	texture = texture_;
	view = texture->get_as_rtv();
}

void SingleRenderTarget::start_render_target(Reference<DepthStencilTexture> depthStencil) {
	texture->start_write();
	auto&& commandList = DxCommand::GetCommandList();
	commandList->OMSetRenderTargets(
		1, &view->handle(),
		depthStencil ? 1 : 0,
		depthStencil ? &depthStencil->get_as_dsv()->handle() : nullptr
	);
}

void SingleRenderTarget::clear_render_target() {
	view->clear();
}
