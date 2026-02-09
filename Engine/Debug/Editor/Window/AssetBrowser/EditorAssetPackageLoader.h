#pragma once

#include <filesystem>

namespace szg {

class EditorAssetPackageLoader final {
public:
	void add_package(const std::filesystem::path& filePath);

	void update();

private:
	void show_load_popup();

	void execute_load();

public:
	struct PackageLoadInfo {
		std::filesystem::path filePath;
		bool useOptimization{ false };
	};

private:
	bool isShowLoadPopup{ false };
	std::vector<PackageLoadInfo> loadAssets;
};

} // namespace szg
