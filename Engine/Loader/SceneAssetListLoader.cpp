#include "SceneAssetListLoader.h"

#include "Engine/Assets/AssetTypeEnum.h"
#include "Engine/Assets/Json/JsonAsset.h"

using namespace szg;

SceneAssetCollection SceneAssetListLoader::load(const std::string& sceneName) {
	SceneAssetCollection::AssetListType assets{};
	SceneAssetCollection::AssetListType lazyAssets{};

	JsonAsset json{ std::format("./Game/Core/Scene/{}/Assets.json", sceneName) };

	if (json.cget().is_null()) {
		return {};
	}

	load_asset_list(assets, json.cget());
	load_asset_list(lazyAssets, json.cget().value("LazyLoadAssets", nlohmann::json::object()));

	SceneAssetCollection collection{ assets, lazyAssets };

	return collection;
}

void SceneAssetListLoader::load_asset_list(SceneAssetCollection::AssetListType& assets, const nlohmann::json& json) {
	for (i32 i = 0; i < SceneAssetCollection::COLLECTION_ASSET_TYPE_MAX; ++i) {
		auto& assetSet = assets[i];

		const nlohmann::json& assetArray = json.value(ASSET_TYPE_NAME[i + 1], nlohmann::json::array());
		for (const std::filesystem::path& assetPath : assetArray.get<std::vector<std::filesystem::path>>()) {
			assetSet.emplace(assetPath);
		}
	}
}
