#pragma once

#include <array>
#include <filesystem>
#include <unordered_set>

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Assets/AssetTypeEnum.h"

namespace szg {

class SceneAssetCollection {
#ifdef DEBUG_FEATURES_ENABLE
	friend class EditorAssetSaver;
#endif // DEBUG_FEATURES_ENABLE

public:
	static constexpr i32 COLLECTION_ASSET_TYPE_MAX = ASSET_TYPE_MAX - 1;
	using AssetListType = std::array<std::unordered_set<std::filesystem::path>, COLLECTION_ASSET_TYPE_MAX>;

public:
	SceneAssetCollection() = default;
	~SceneAssetCollection() = default;

	SZG_CLASS_MOVE_ONLY(SceneAssetCollection)

	SceneAssetCollection(const AssetListType& assets, const AssetListType& lazyLoadAssets);

public:
	/// <summary>
	/// 通常ロードの開始
	/// </summary>
	void load_assets() const;

	/// <summary>
	/// 遅延ロードの開始
	/// </summary>
	void load_lazy_assets() const;

private:
	/// <summary>
	/// BackgroundLoaderに登録する
	/// </summary>
	/// <param name="assets"></param>
	static void RegisterLoadQueue(const AssetListType& assets);

private:
	AssetListType assets;
	AssetListType lazyLoadAssets;
};

}; // szg
