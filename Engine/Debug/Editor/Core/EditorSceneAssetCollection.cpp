#ifdef DEBUG_FEATURES_ENABLE

#include "EditorSceneAssetCollection.h"

void szg::EditorSceneAssetCollection::RegisterAsset(AssetType type, const std::filesystem::path& assetFilePath, bool isLazy) {
	auto& instance = GetInstance();
	i32 idx = static_cast<i32>(type) - 1;
	if (instance.assets[idx].contains(assetFilePath)) {
		++instance.assets[idx][assetFilePath];
	}
	else {
		instance.assets[idx][assetFilePath] = 1;
	}
	instance.isLazyLoadAsset[idx][assetFilePath.filename().string()] |= isLazy;
}

void szg::EditorSceneAssetCollection::UnregisterAsset(AssetType type, const std::filesystem::path& assetFilePath) {
	auto& instance = GetInstance();
	i32 idx = static_cast<i32>(type) - 1;
	i32& counter = instance.assets[idx][assetFilePath];
	--counter;
	if (counter <= 0) {
		instance.assets[idx].erase(assetFilePath);
		instance.isLazyLoadAsset[idx].erase(assetFilePath.filename().string());
	}
}

void szg::EditorSceneAssetCollection::Clear() {
	auto& instance = GetInstance();
	for (auto& assetSet : instance.assets) {
		assetSet.clear();
	}
	for (auto& lazyMap : instance.isLazyLoadAsset) {
		lazyMap.clear();
	}
}

bool szg::EditorSceneAssetCollection::IsLazyAsset(AssetType type, const std::filesystem::path& assetFilePath) {
	auto& instance = GetInstance();
	auto& lazyMap = instance.isLazyLoadAsset[static_cast<size_t>(type)];
	std::string filename = assetFilePath.filename().string();
	if (lazyMap.contains(filename)) {
		return lazyMap[filename];
	}
	return false;
}

const std::array<std::unordered_map<std::filesystem::path, i32>, szg::SceneAssetCollection::COLLECTION_ASSET_TYPE_MAX>& szg::EditorSceneAssetCollection::AssetsImm() noexcept {
	return GetInstance().assets;
}

const std::array<std::unordered_map<std::string, bool>, szg::SceneAssetCollection::COLLECTION_ASSET_TYPE_MAX>& szg::EditorSceneAssetCollection::IsLazyLoadAssetsImm() noexcept {
	return GetInstance().isLazyLoadAsset;
}

#endif // DEBUG_FEATURES_ENABLE
