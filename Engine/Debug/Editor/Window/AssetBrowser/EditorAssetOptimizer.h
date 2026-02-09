#pragma once

#include <filesystem>

namespace szg {

class EditorAssetOptimizer final {
public:
	void optimize_asset(const std::filesystem::path& inputFile, const std::filesystem::path& outputPath);

	bool is_support_optimization(const std::filesystem::path& extension) const;
};

}
