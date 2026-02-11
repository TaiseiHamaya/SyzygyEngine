#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <filesystem>
#include <optional>

#include "./EditorAssetOptimizer.h"

namespace szg {

class EditorAssetImporter final {
public:
	void add_package(const std::filesystem::path& filePath);

	void update();

	std::optional<std::filesystem::path>& import_filepath_mut();

private:
	void show_load_popup();

	void execute_import();

public:
	struct PackageLoadInfo {
		std::filesystem::path filePath;
		bool isSupportOptimization;
		bool useOptimization{ false };
	};

private:
	bool isShowLoadPopup{ false };

	std::vector<PackageLoadInfo> loadAssets;

	std::optional<std::filesystem::path> importFilePath;

	EditorAssetOptimizer optimizer;
};

} // namespace szg

#endif // DEBUG_FEATURES_ENABLE
