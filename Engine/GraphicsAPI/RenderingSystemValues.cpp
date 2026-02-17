#include "RenderingSystemValues.h"

using namespace szg;

Reference<DepthStencilTexture> RenderingSystemValues::GetDepthStencilTexture() {
	return texture;
}

void RenderingSystemValues::Initialize() {
	texture = std::make_unique<DepthStencilTexture>();
	texture->initialize(DXGI_FORMAT_D32_FLOAT);
}

void RenderingSystemValues::Finalize() {
	texture.reset();
}
