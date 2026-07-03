#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "./CSVAsset.h"

namespace szg {

class CSVAssetBuilder final {
public:
	CSVAssetBuilder(const std::string& split_ = ",") noexcept;
	~CSVAssetBuilder() = default;

	SZG_CLASS_MOVE_ONLY(CSVAssetBuilder);

public:
	template<typename T>
	std::optional<CSVAsset<T>> load_from_file(const std::filesystem::path& file);

private:
	bool load_as_string();

private:
	const std::string split;
	std::filesystem::path filePath;
	std::vector<std::vector<std::string>> temp;
};

}; // namespace szg

#include "CSVAssetBuilder.inl"
