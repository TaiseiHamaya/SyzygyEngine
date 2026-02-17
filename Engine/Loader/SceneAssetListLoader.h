#pragma once

#include <json.hpp>

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Runtime/Scene/SceneAssetCollection.h"

namespace szg {

class SceneAssetListLoader {
public:
	SceneAssetListLoader() = default;
	~SceneAssetListLoader() = default;

	SZG_CLASS_MOVE_ONLY(SceneAssetListLoader)

public:
	SceneAssetCollection load(const std::string& sceneName);

private:
	void load_asset_list(
		SceneAssetCollection::AssetListType& assets,
		const nlohmann::json& json
	);
};

}; // szg
