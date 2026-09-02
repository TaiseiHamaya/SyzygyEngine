#pragma once

#include "CSVAssetSaver.h"

#include "Engine/Assets/CSV/CSVAsset.h"

template<typename T>
inline szg::CSVAssetSaver<T>::CSVAssetSaver(const CSVAsset<T>& csv_) : csv(csv_) {
}

template<typename T>
inline void szg::CSVAssetSaver<T>::save_to(const std::filesystem::path& filepath) const {
	std::string result;
	for (i64 r = 0; r < csv->size_row(); ++r) {
		for (i64 c = 0; c < csv->size_col(r); ++c) {
			result += std::to_string(csv->at(r, c));
			result += ',';
		}
		result.back() = '\n';
	}
	result.pop_back();

	std::ofstream ofs(filepath, std::ios::out | std::ios::trunc);
	ofs << result;
	ofs.close();
}