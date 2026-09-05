#pragma once

// implement inline function

#include "CSVAsset.h"
#include "Engine/Application/Logger.h"

template<typename T>
inline szg::CSVAsset<T>::CSVAsset(std::vector<std::vector<T>> data_) :
	data(data_) {
}

template<typename T>
inline const T& szg::CSVAsset<T>::at(i64 row, i64 col) const {
	szgAssert(row >= 0 && row < static_cast<i64>(data.size()));
	auto& rows = data[row];
	szgAssert(col >= 0 && col < static_cast<i64>(rows.size()));
	return rows[col];
}

template<typename T>
inline T& szg::CSVAsset<T>::at(i64 row, i64 col) {
	szgAssert(row >= 0 && row < static_cast<i64>(data.size()));
	auto& rows = data[row];
	szgAssert(col >= 0 && col < static_cast<i64>(rows.size()));
	return rows[col];
}

template<typename T>
inline T szg::CSVAsset<T>::at_unchecked(i64 row, i64 col) const {
	return data[row][col];
}

template<typename T>
inline i64 szg::CSVAsset<T>::size_row() const {
	return static_cast<i64>(data.size());
}

template<typename T>
inline i64 szg::CSVAsset<T>::size_col(i64 row) const {
	szgAssert(row >= 0 && row < static_cast<i64>(data.size()));
	return static_cast<i64>(data[row].size());
}

template<typename T>
inline void szg::CSVAsset<T>::resize_all(i64 row, i64 col) {
	data.resize(row, std::vector<T>(col));
	for (i64 r = 0; r < row; ++r) {
		data[r].resize(col);
	}
}

template<typename T>
inline void szg::CSVAsset<T>::resize_row(i64 row, i64 size) {
	data.resize(row, std::vector<T>(size));
}

template<typename T>
inline void szg::CSVAsset<T>::resize_col(i64 row, i64 col) {
	szgAssert(row >= 0 && row < static_cast<i64>(data.size()));
	data[row].resize(col);
}
