#pragma once

#include "Engine/Assets/CSV/CSVAsset.h"

#include <filesystem>
#include <Library/Utility/Template/Reference.h>

namespace szg {

template<typename T>
class CSVAssetSaver {
public:
	CSVAssetSaver(const CSVAsset<T>& csv_);
	~CSVAssetSaver() = default;

	SZG_CLASS_MOVE_ONLY(CSVAssetSaver<T>);

public:
	void save_to(const std::filesystem::path& filepath) const;

private:

private:
	Reference<const CSVAsset<T>> csv;
};

}; // namespace szg

#include "CSVAssetSaver.inl"
