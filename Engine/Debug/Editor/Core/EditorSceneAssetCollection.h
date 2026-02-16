#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <array>
#include <filesystem>
#include <unordered_map>

#include <Library/Utility/Template/SingletonInterface.h>

#include "Engine/Runtime/Scene/SceneAssetCollection.h"

namespace szg {

class EditorSceneAssetCollection final : public SingletonInterface<EditorSceneAssetCollection> {
	SZG_CLASS_SINGLETON(EditorSceneAssetCollection)

public:
	static void RegisterAsset(AssetType type, const std::filesystem::path& assetFilePath, bool isLazy = false);

	static void UnregisterAsset(AssetType type, const std::filesystem::path& assetFilePath);

	static void Clear();

	static bool IsLazyAsset(AssetType type, const std::filesystem::path& assetFilePath);

public:
	static const std::array<std::unordered_map<std::filesystem::path, i32>, SceneAssetCollection::COLLECTION_ASSET_TYPE_MAX>& AssetsImm() noexcept;

	static const std::array<std::unordered_map<std::string, bool>, SceneAssetCollection::COLLECTION_ASSET_TYPE_MAX>& IsLazyLoadAssetsImm() noexcept;

private:
	/// <summary>
	/// Key: FilePath, Value: Counter
	/// </summary>
	std::array<std::unordered_map<std::filesystem::path, i32>, SceneAssetCollection::COLLECTION_ASSET_TYPE_MAX> assets;
	/// <summary>
	/// Key: FileName, Value: IsLazyLoad
	/// </summary>
	std::array<std::unordered_map<std::string, bool>, SceneAssetCollection::COLLECTION_ASSET_TYPE_MAX> isLazyLoadAsset;
};

} // namespace szg

#endif // DEBUG_FEATURES_ENABLE
