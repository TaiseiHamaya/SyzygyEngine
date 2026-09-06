#include "SceneAssetCollection.h"

#include <filesystem>
#include <functional>

#include "Engine/Assets/Animation/NodeAnimation/NodeAnimationLibrary.h"
#include "Engine/Assets/Animation/Skeleton/SkeletonLibrary.h"
#include "Engine/Assets/Audio/AudioLibrary.h"
#include "Engine/Assets/FontAtlasMSDF/FontAtlasMSDFLibrary.h"
#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"
#include "Engine/Assets/Shader/ShaderLibrary.h"
#include "Engine/Assets/Texture/TextureLibrary.h"

using namespace szg;

SceneAssetCollection::SceneAssetCollection(const AssetListType& assets_, const AssetListType& lazyLoadAssets_) {
	assets = assets_;
	lazyLoadAssets = lazyLoadAssets_;
}

void SceneAssetCollection::load_assets() const {
	RegisterLoadQueue(assets);
}

void SceneAssetCollection::load_lazy_assets() const {
	RegisterLoadQueue(lazyLoadAssets);
}

void SceneAssetCollection::RegisterLoadQueue(const AssetListType& assets) {
	// 関数オブジェクトの配列
	const std::array<std::function<void(const std::filesystem::path&)>, SceneAssetCollection::COLLECTION_ASSET_TYPE_MAX> loadFunc{
		TextureLibrary::RegisterLoadQue,
		PolygonMeshLibrary::RegisterLoadQue,
		SkeletonLibrary::RegisterLoadQue,
		NodeAnimationLibrary::RegisterLoadQue,
		FontAtlasMSDFLibrary::RegisterLoadQue,
		nullptr,
		AudioLibrary::RegisterLoadQue,
		ShaderLibrary::RegisterLoadQue,
		nullptr,
		nullptr,
	};

	for (u32 i = 0; i < SceneAssetCollection::COLLECTION_ASSET_TYPE_MAX; ++i) {
		const std::unordered_set<std::filesystem::path>& assetSet = assets[i];
		for (const std::filesystem::path& assetPath : assetSet) {
			if (loadFunc[i]) {
				loadFunc[i](assetPath);
			}
		}
	}
}
