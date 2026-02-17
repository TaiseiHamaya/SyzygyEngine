#include "RenderTargetCollection.h"

#include "Engine/Module/Render/RenderPipeline/Deferred/DeferredAdaptor.h"
#include "Engine/Module/Render/RenderTargetGroup/SingleRenderTarget.h"
#include <Engine/Application/Logger.h>

using namespace szg;

std::pair<Reference<BaseRenderTargetGroup>, Reference<RenderTexture>> RenderTargetCollection::create_render_target_group(const nlohmann::json& json) {
	std::unique_ptr<SingleRenderTarget> result;

	u32 width = json.value("OutputSize", nlohmann::json::object()).value("X", ProjectSettings::ClientWidth());
	u32 height = json.value("OutputSize", nlohmann::json::object()).value("Y", ProjectSettings::ClientHeight());

	auto& texture = renderTextures.emplace_back();
	texture = std::make_unique<RenderTexture>();
	texture->initialize(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, width, height);
	szgInformation("Output Texture {} created. Size-{},{} (Format: {})", renderTextures.size(), width, height, static_cast<u32>(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB));
	result = std::make_unique<SingleRenderTarget>();
	result->initialize(texture);

	renderTargets.emplace_back(std::move(result));
	return std::make_pair<Reference<BaseRenderTargetGroup>, Reference<RenderTexture>>(renderTargets.back(), texture);
}

std::pair<Reference<DeferredAdaptor::GBuffersType>, std::array<Reference<RenderTexture>, DeferredAdaptor::NUM_GBUFFER>> RenderTargetCollection::create_gbuffer(const nlohmann::json& json) {
	std::unique_ptr<DeferredAdaptor::GBuffersType> result = std::make_unique<DeferredAdaptor::GBuffersType>();
	Reference<DeferredAdaptor::GBuffersType> temp = result;
	std::array<Reference<RenderTexture>, DeferredAdaptor::NUM_GBUFFER> gbufferTextures;

	u32 width = json.value("GBufferSize", nlohmann::json::object()).value("X", ProjectSettings::ClientWidth());
	u32 height = json.value("GBufferSize", nlohmann::json::object()).value("Y", ProjectSettings::ClientHeight());

	for (i32 i = 0; i < DeferredAdaptor::NUM_GBUFFER; ++i) {
		gbufferTextures[i] = renderTextures.emplace_back(std::make_unique<RenderTexture>());
		gbufferTextures[i]->initialize(DeferredAdaptor::DXGI_FORMAT_LIST[i], width, height);
		szgInformation("GBuffer Texture {} created. Size-{},{} (Format: {})", renderTextures.size(), width, height, static_cast<u32>(DeferredAdaptor::DXGI_FORMAT_LIST[i]));
	}
	result->initialize(gbufferTextures);
	renderTargets.emplace_back(std::move(result));
	return std::make_pair(temp, gbufferTextures);
}

void RenderTargetCollection::clear() {
	renderTargets.clear();
	renderTextures.clear();
}
