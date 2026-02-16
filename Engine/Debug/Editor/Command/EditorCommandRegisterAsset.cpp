#ifdef DEBUG_FEATURES_ENABLE

#include "EditorCommandRegisterAsset.h"

#include "Engine/Debug/Editor/Core/EditorSceneAssetCollection.h"

szg::EditorCommandRegisterAsset::EditorCommandRegisterAsset(AssetType type_, const std::filesystem::path& assetFilePath_, bool isLazy_) :
	type(type_), 
	assetFilePath(assetFilePath_),
	isLazy(isLazy_) {
}

void szg::EditorCommandRegisterAsset::execute() {
	szg::EditorSceneAssetCollection::RegisterAsset(type, assetFilePath, isLazy);
}

void szg::EditorCommandRegisterAsset::undo() {
	szg::EditorSceneAssetCollection::UnregisterAsset(type, assetFilePath);
}

szg::EditorCommandUnregisterAsset::EditorCommandUnregisterAsset(AssetType type_, const std::filesystem::path& assetFilePath_) :
	type(type_),
	assetFilePath(assetFilePath_) {
	isLazy = szg::EditorSceneAssetCollection::IsLazyAsset(type, assetFilePath);
}

void szg::EditorCommandUnregisterAsset::execute() {
	szg::EditorSceneAssetCollection::UnregisterAsset(type, assetFilePath);
}

void szg::EditorCommandUnregisterAsset::undo() {
	szg::EditorSceneAssetCollection::RegisterAsset(type, assetFilePath, isLazy);
}

#endif // DEBUG_FEATURES_ENABLE
