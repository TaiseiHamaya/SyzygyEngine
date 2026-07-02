#pragma once

#include <vector>

#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

template<typename T>
class CSVAsset final {
public:
	CSVAsset(std::vector<std::vector<T>> data_ = {});
	~CSVAsset() = default;

	SZG_CLASS_MOVE_ONLY(CSVAsset<T>);

public:
	const T& at(i64 row, i64 col) const;
	T& at(i64 row, i64 col);

	T at_unchecked(i64 row, i64 col) const;

	i64 size_row() const;
	i64 size_col(i64 row) const;

	void resize_all(i64 row, i64 col);

	void resize_row(i64 row, i64 size = 0);

	void resize_col(i64 row, i64 col);

private:
	std::vector<std::vector<T>> data;
};

}; // namespace szg

#include "CSVAsset.inl"
