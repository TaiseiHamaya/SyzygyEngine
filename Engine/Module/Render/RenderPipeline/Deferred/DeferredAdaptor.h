#pragma once

#include "Engine/Module/Render/RenderTargetGroup/MultiRenderTarget.h"

namespace szg {

namespace DeferredAdaptor {

/// <summary>
/// G-Bufferの数
/// </summary>
inline constexpr u32 NUM_GBUFFER = 2;

/// <summary>
/// MRTの型
/// </summary>
using GBuffersType = MultiRenderTarget<NUM_GBUFFER>;

/// <summary>
/// DXGI_FORMATの一覧
/// </summary>
inline constexpr std::array<DXGI_FORMAT, NUM_GBUFFER> DXGI_FORMAT_LIST = {
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R32_UINT,
};

} // namespace DeferredAdaptor

} // szg
