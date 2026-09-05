#ifdef DEBUG_FEATURES_ENABLE

#include "EditorAssetSaver.h"

#include <memory>
#include <ranges>

#include "Engine/Assets/Json/JsonAsset.h"
#include "Engine/Debug/Editor/Core/EditorSceneAssetCollection.h"
#include "Engine/Debug/Editor/Window/RenderDAG/EditorRenderDAG.h"
#include "Engine/Debug/Editor/Window/RenderDAG/PostEffectImNode.h"
#include "Engine/Loader/RenderPath/RenderNodeType.h"

using namespace szg;

void EditorAssetSaver::setup(Reference<const EditorRenderDAG> dagEditor_) {
	dagEditor = dagEditor_;
}

void EditorAssetSaver::save(const std::filesystem::path& filePath) {
	collect_assets();

	collect_shaders();

	save_to_json(filePath);
}

void szg::EditorAssetSaver::collect_assets() {
	auto& assets = EditorSceneAssetCollection::AssetsImm();
	auto& isLazy = EditorSceneAssetCollection::IsLazyLoadAssetsImm();
	for (i32 i = 0; i < SceneAssetCollection::COLLECTION_ASSET_TYPE_MAX; ++i) {
		for (const auto& asset : assets[i] | std::views::keys) {
			std::string filename = asset.filename().string();
			if (isLazy[i].contains(filename) && isLazy[i].at(filename)) {
				sceneAssetCollection.lazyLoadAssets[i].emplace(asset);
			}
			else {
				sceneAssetCollection.assets[i].emplace(asset);
			}
		}
	}
}

void EditorAssetSaver::collect_shaders() {
	if (!dagEditor) {
		return;
	}

	const std::unordered_map<u64, EditorRenderDAG::DAGNodeType>& nodes = dagEditor->nodes_imm();

	for (const auto& [id, node] : nodes) {
		RenderNodeType nodeType = static_cast<RenderNodeType>(node.index());
		if (nodeType != RenderNodeType::PostEffect) {
			continue;
		}

		std::shared_ptr<PostEffectImNode> postEffectNode = std::get<std::shared_ptr<PostEffectImNode>>(node);
		PostEffectType peType = postEffectNode->post_effect_type();
		std::filesystem::path pixelShaderFile;
		switch (peType) {
		case PostEffectType::Bloom:
			pixelShaderFile = L"[[szg]]/PostEffect/Bloom.PS.hlsl";
			break;
		case PostEffectType::ChromaticAberration:
			pixelShaderFile = L"[[szg]]/PostEffect/ChromaticAberration.PS.hlsl";
			break;
		case PostEffectType::GaussianBlur:
			pixelShaderFile = L"[[szg]]/PostEffect/GaussianBlur.PS.hlsl";
			break;
		case PostEffectType::Grayscale:
			pixelShaderFile = L"[[szg]]/PostEffect/Grayscale.PS.hlsl";
			break;
		case PostEffectType::LuminanceExtraction:
			pixelShaderFile = L"[[szg]]/PostEffect/LuminanceExtraction.PS.hlsl";
			break;
		case PostEffectType::TextureBlend4:
			pixelShaderFile = L"[[szg]]/PostEffect/MargeTexture4.PS.hlsl";
			break;
		case PostEffectType::Outline:
			pixelShaderFile = L"[[szg]]/PostEffect/Outline.PS.hlsl";
			break;
		case PostEffectType::RadialBlur:
			pixelShaderFile = L"[[szg]]/PostEffect/RadialBlur.PS.hlsl";
			break;
		default:
			continue;
			break;
		}

		sceneAssetCollection.assets[static_cast<i32>(AssetType::Shader) - 1].emplace(pixelShaderFile.string());
	}
}

void EditorAssetSaver::save_to_json(const std::filesystem::path& filePath) {
	JsonAsset json{ filePath / "Assets.json" };

	nlohmann::json assets = nlohmann::json::object();
	for (u32 i = 0; i < SceneAssetCollection::COLLECTION_ASSET_TYPE_MAX; ++i) {
		const std::unordered_set<std::filesystem::path>& assetList = sceneAssetCollection.assets[i];
		nlohmann::json assetArray = nlohmann::json::array();
		for (const auto& asset : assetList) {
			assetArray.emplace_back(asset);
		}
		assets[ASSET_TYPE_NAME[i + 1]] = assetArray;
	}

	json.get().clear();
	json.get() = assets;

	json.save();
}

#endif // DEBUG_FEATURES_ENABLE
