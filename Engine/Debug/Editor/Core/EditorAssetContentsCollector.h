#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <array>
#include <filesystem>
#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

#include <Library/Utility/Template/SingletonInterface.h>

#include "Engine/Assets/AssetTypeEnum.h"
#include "Engine/Runtime/Clock/WorldTimer.h"

namespace szg {

class EditorAssetContentsCollector final : public SingletonInterface<EditorAssetContentsCollector> {
	SZG_CLASS_SINGLETON(EditorAssetContentsCollector)

private:
	struct AssetEntry {
		AssetType type{ AssetType::Unknown };
		std::string fileName;
		std::string extension;
		std::filesystem::path path;

		std::function<void(const std::string&)> loadFunc;
	};

public:
	static void Setup();

	static void Finalize();

	static void Update();

	static std::optional<AssetEntry> ComboGUI(const std::string& current, AssetType type, const std::string& label = "");

	static AssetType GetAssetTypeByExtension(const std::string& extension);

	static std::optional<std::filesystem::path> GetAssetPath(AssetType type, const std::string& fileName);

private:
	void collect_assets();

private:
	static inline std::mutex mutex{};

private:
	std::array<std::map<std::string, AssetEntry>, ASSET_TYPE_MAX> assetMaps{};

	std::thread thread;
	WorldTimer timer;
};

} // namespace szg

#endif // DEBUG_FEATURES_ENABLE
