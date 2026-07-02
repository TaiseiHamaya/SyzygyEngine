#pragma once

#include "CSVAssetBuilder.h"

#include <ranges>

szg::CSVAssetBuilder::CSVAssetBuilder(const std::string& split_) noexcept :
	split(split_) {
}

bool szg::CSVAssetBuilder::load_as_string() {
	std::ifstream ifstream{ filePath };
	if (!ifstream.is_open()) {
		szgWarning("Failed to open CSV file: {}", filePath.string());
		return false;
	}

	std::string line;
	while (std::getline(ifstream, line)) {
		std::vector<std::string> row;
		for (auto cell : line | std::views::split(split)) {
			row.emplace_back(cell.begin(), cell.end());
		}
		temp.emplace_back(row);
	}

	ifstream.close();

	return true;
}
