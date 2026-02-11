#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <filesystem>

namespace szg {

class EditorAssetOptimizer final {
public:
	void optimize_asset(const std::filesystem::path& inputFile, const std::filesystem::path& outputPath);

	bool is_support_optimization(const std::filesystem::path& extension) const;
};

}

#endif // DEBUG_FEATURES_ENABLE
