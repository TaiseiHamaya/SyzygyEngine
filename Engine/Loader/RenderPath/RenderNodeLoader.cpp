#include "RenderNodeLoader.h"

using namespace szg;

#include "./PostEffectPSOLoader.h"
#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/GraphicsAPI/DirectX/DxSwapChain/DxSwapChain.h"
#include "Engine/Loader/RenderPath/RenderNodeType.h"
#include "Engine/Module/Render/RenderNode/PostEffectNode.h"
#include "Engine/Module/Render/RenderNode/WorldLayerRenderNode.h"
#include "Engine/Module/Render/RenderTargetCollection/RenderTargetCollection.h"
#include "Engine/Module/Render/RenderTargetGroup/SwapChainRenderTargetGroup.h"
#include "Engine/Runtime/Scene/Scene.h"

#define VECTOR2_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

RenderNodeLoader::RenderNodeLoader() = default;
RenderNodeLoader::~RenderNodeLoader() noexcept = default;

void RenderNodeLoader::setup(Reference<Scene> scene_, Reference<RenderTargetCollection> renderTargetCollection_) {
	scene = scene_;
	renderTargetCollection = renderTargetCollection_;
	pePsoLoader = std::make_unique<PostEffectPSOLoader>();
	pePsoLoader->setup(immidiateData);
}

std::vector<std::unique_ptr<IRenderNode>> RenderNodeLoader::entry_point(const nlohmann::json& json) {
	calclate_result_node_index(json);

	for (auto& node : json["Nodes"]) {
		if (!node.contains("Type")) {
			szgWarning("RenderNodeLoader::entry_point: Node Type is not specified.");
			continue;
		}
		const RenderNodeType type = node.value("Type", RenderNodeType::Unknown);
		switch (type) {
		case RenderNodeType::WorldLayer:
		{
			auto temp = load_as_world_render(node);
			result.emplace_back(std::move(temp));
			break;
		}
		case RenderNodeType::PostEffect:
		{
			auto temp = load_as_post_effect(node);
			result.emplace_back(std::move(temp));
			break;
		}
		case RenderNodeType::StaticTexture:
		{
			load_as_static_texture(node);
			break;
		}
		default:
			szgWarning("RenderNodeLoader::entry_point: Unknown Node Type \'{}\'", static_cast<i32>(type));
			break;
		}
	}

	return std::move(result);
}
std::unique_ptr<IRenderNode> RenderNodeLoader::load_as_post_effect(const nlohmann::json& json) {
	std::unique_ptr<PostEffectNode> node = std::make_unique<PostEffectNode>();

	const nlohmann::json& dataJson = json["Data"];
	PostEffectNode::Data resultData{};
	{
		resultData.rect = {
			0, 0,
			dataJson["OutputSize"]["X"].get<LONG>(),
			dataJson["OutputSize"]["Y"].get<LONG>()
		};
		resultData.viewport = {
			0.0f, 0.0f,
			static_cast<FLOAT>(dataJson["OutputSize"]["X"].get<u32>()),
			static_cast<FLOAT>(dataJson["OutputSize"]["Y"].get<u32>()),
			0.0f,
			1.0f
		};
		resultData.postEffectPSO = pePsoLoader->entry_point(json);
		if (resultNodeIndex <= result.size()) {
			Reference<SwapChainRenderTargetGroup> swapChainRT = DxSwapChain::GetRenderTarget();
			immidiateData.emplace_back(swapChainRT, nullptr);
			resultData.outputRenderTargetGroup = swapChainRT;
		}
		else {
			auto [renderTargetGroup, renderTexture] = renderTargetCollection->create_render_target_group(dataJson);
			immidiateData.emplace_back(renderTargetGroup, renderTexture);
			resultData.outputRenderTargetGroup = renderTargetGroup;
		}
	}

	node->set_data(std::move(resultData));

	return node;
}

std::unique_ptr<IRenderNode> RenderNodeLoader::load_as_world_render(const nlohmann::json& json) {
	std::unique_ptr<WorldLayerRenderNode> node = std::make_unique<WorldLayerRenderNode>();
	WorldLayerRenderNode::Data resultData{};

	const nlohmann::json& dataJson = json["Data"];
	const nlohmann::json& linkJson = json["Links"];
	{ // GBuffer
		resultData.gBuffer.rect = {
			0, 0,
			dataJson["GBufferSize"]["X"].get<LONG>(),
			dataJson["GBufferSize"]["Y"].get<LONG>()
		};
		resultData.gBuffer.viewport = {
			dataJson["Left"].get<FLOAT>(),
			dataJson["Top"].get<FLOAT>(),
			dataJson["Width"].get<FLOAT>(),
			dataJson["Height"].get<FLOAT>(),
			dataJson["MinDepth"].get<FLOAT>(),
			dataJson["MaxDepth"].get<FLOAT>()
		};
	}

	{ // Layer
		resultData.layerData.rect = {
			dataJson["TopLeft"]["X"].get<LONG>(),
			dataJson["TopLeft"]["Y"].get<LONG>(),
			dataJson["BottomRight"]["X"].get<LONG>(),
			dataJson["BottomRight"]["Y"].get<LONG>()
		};
		resultData.layerData.viewport = {
			0.0f, 0.0f,
			dataJson["BottomRight"]["X"].get<r32>() - dataJson["TopLeft"]["X"].get<r32>(),
			dataJson["BottomRight"]["Y"].get<r32>() - dataJson["TopLeft"]["Y"].get<r32>(),
			0.0f,
			1.0f
		};
		resultData.layerData.outputTextureSize =
			dataJson.value("OutputSize", Vector2{ 1920, 1080 });
		const u32 worldIndex = dataJson.value("WorldIndex", 0);
		resultData.layerData.index = dataJson.value<u8>("LayerIndex", 0);
		resultData.layerData.worldRenderCollection = scene->get_world(worldIndex)->render_collection();

		resultData.layerData.cameraId = dataJson.value("CameraId", 0);

		resultData.layerData.isClearDepthStencil = dataJson.value("ClearDepthStencil", true);
	}

	{ // output
		auto [gbufferRT, gbufferTexture] = renderTargetCollection->create_gbuffer(dataJson);
		resultData.gBuffer.renderTarget = gbufferRT;
		resultData.gBuffer.texture = gbufferTexture;

		if (resultNodeIndex <= result.size()) {
			// SwapChainに描画する場合
			Reference<SwapChainRenderTargetGroup> swapChainRT = DxSwapChain::GetRenderTarget();
			immidiateData.emplace_back(swapChainRT, nullptr);
			resultData.outputRenderTargetGroup = swapChainRT;
			if (!linkJson.contains("Base")) {
				resultData.layerData.isClearRenderTarget = true;
			}
		}
		else if (linkJson.contains("Base")) {
			// Link先のTextureを継続使用
			u64 index = linkJson["Base"];
			immidiateData.emplace_back(immidiateData[index]);
			resultData.outputRenderTargetGroup = immidiateData[index].renderTargetGroup;
		}
		else {
			// ない場合
			auto [renderTargetGroup, renderTexture] = renderTargetCollection->create_render_target_group(dataJson);
			resultData.outputRenderTargetGroup = renderTargetGroup;
			immidiateData.emplace_back(
				renderTargetGroup,
				renderTexture
			);
			resultData.layerData.isClearRenderTarget = true;
		}
	}

	node->setup(std::move(resultData));

	return node;
}

void RenderNodeLoader::load_as_static_texture([[maybe_unused]] const nlohmann::json& json) {
	immidiateData.emplace_back(
		nullptr,
		nullptr
	);
}

void RenderNodeLoader::calclate_result_node_index(const nlohmann::json& json) {
	i32 index = json.value("ResultNode", nlohmann::json::object()).value("Connect", 0) - 1;

	const nlohmann::json& nodeJson = json["Nodes"];
	while (true) {
		auto& node = nodeJson.at(index);
		if (node.value("Type", RenderNodeType::Unknown) != RenderNodeType::WorldLayer) {
			break;
		}
		const nlohmann::json& linksJson = node.value("Links", nlohmann::json::object());
		if (!linksJson.contains("Base")) {
			break;
		}
		index = linksJson["Base"].get<i32>();

	}
	resultNodeIndex = index;
}
