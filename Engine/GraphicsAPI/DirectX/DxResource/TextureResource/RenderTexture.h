#pragma once

#include "./ITextureResource.h"

#include <Library/Math/ColorRGBA.h>

#include "./IResourceView/RenderTargetView.h"
#include "./IResourceView/ShaderResourceView.h"
#include "Engine/Application/ProjectSettings/ProjectSettings.h"

namespace szg {

class RenderTexture final : public ITextureResource {
public:
	RenderTexture() = default;
	~RenderTexture();

	SZG_CLASS_MOVE_ONLY(RenderTexture)

public:
	void initialize(DXGI_FORMAT format, u32 width_ = ProjectSettings::ClientWidth(), u32 height_ = ProjectSettings::ClientHeight());
	void start_write();
	void start_read();

	Reference<const RenderTargetView> get_as_rtv() const { return rtv; }
	Reference<const ShaderResourceView> get_as_srv() const { return srv; }

private:
	void create_resource(DXGI_FORMAT format);

private:
	ColorRGBA color;
	RenderTargetView rtv;
	ShaderResourceView srv;
};

}; // szg
