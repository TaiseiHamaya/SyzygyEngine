#pragma once

#include "CSVAssetBuilder.h"

#include <charconv>

#include "../IAssetBuilder.h"

template<typename T>
inline std::optional<szg::CSVAsset<T>> szg::CSVAssetBuilder::load_from_file(const std::filesystem::path& file) {
	filePath = IAssetBuilder::ResolveFilePath(file, "csv");
	if (!load_as_string()) {
		return std::nullopt;
	}

	if constexpr (std::is_same_v<T, std::string>) {
		// 文字列型の場合はそのまま返す
		return szg::CSVAsset{ temp };
	}

	std::vector<std::vector<T>> data;
	for (const auto& row : temp) {
		std::vector<T> convertedRow;
		for (const auto& cell : row) {
			if constexpr (std::is_arithmetic_v<T>) {
				T value{};
				std::from_chars(std::to_address(cell.begin()), std::to_address(cell.end()), value);
				convertedRow.emplace_back(value);
			}
		}
		data.emplace_back(convertedRow);
	}

	return szg::CSVAsset{ data };
}
