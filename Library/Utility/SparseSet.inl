#pragma once

#include "SparseSet.h"

#include <numeric>

#include "Engine/Application/Logger.h"

template<typename T>
inline szg::SparseSet<T>::SparseSet(i64 size, const T& defaultValue) {
	data.resize(size, defaultValue);
	indexes.resize(size);
	ids.resize(size);

	std::iota(indexes.begin(), indexes.end(), 0);
	std::iota(ids.begin(), ids.end(), 0);
}

template<typename T>
inline Reference<const T> szg::SparseSet<T>::at(i64 id) const {
	if (!is_valid_id(id)) {
		szgError("Invalid id: {}", id);
		return nullptr;
	}
	return data[indexes[id]];
}

template<typename T>
inline Reference<T> szg::SparseSet<T>::at_mut(i64 id) {
	if (!is_valid_id(id)) {
		szgError("Invalid id: {}", id);
		return nullptr;
	}
	return data[indexes[id]];
}

template<typename T>
inline const T& szg::SparseSet<T>::at_unchecked(i64 id) const {
	return data[indexes[id]];
}

template<typename T>
inline T& szg::SparseSet<T>::at_mut_unchecked(i64 id) {
	return data[indexes[id]];
}

template<typename T>
inline i64 szg::SparseSet<T>::size() const {
	return static_cast<i64>(data.size());
}

template<typename T>
inline void szg::SparseSet<T>::clear() {
	data.clear();
	indexes.clear();
	ids.clear();
}

template<typename T>
inline bool szg::SparseSet<T>::empty() const {
	return data.empty();
}

template<typename T>
template<typename ...Args>
inline i64 szg::SparseSet<T>::emplace_back(Args && ...args) {
	i64 dataIdx = size();

	if (dataIdx == id_space_size()) {
		indexes.emplace_back(dataIdx);
		ids.emplace_back(dataIdx);
	}

	data.emplace_back(std::forward<Args>(args)...);

	return ids[dataIdx];
}

template<typename T>
inline void szg::SparseSet<T>::remove(i64 removeId) {
	if (!is_valid_id(removeId)) {
		szgError("Invalid id: {}", removeId);
		return;
	}

	i64 backIdx = size() - 1;
	i64 backId = ids[backIdx];

	if (removeId != backId) {
		i64 removeIdx = indexes[removeId];

		std::swap(data[removeIdx], data[backIdx]);
		std::swap(ids[removeIdx], ids[backIdx]);

		indexes[removeId] = backIdx;
		indexes[backId] = removeIdx;
	}

	data.pop_back();
}

template<typename T>
inline void szg::SparseSet<T>::pop_back() {
	if (empty()) {
		szgWarning("SparseSet is empty, cannot pop_back");
		return;
	}

	remove(ids[size() - 1]);
}

template<typename T>
inline void szg::SparseSet<T>::resize(i64 newSize) {
	if (newSize < 0) {
		szgWarning("Invalid newSize: {}", newSize);
		return;
	}

	if (newSize == size()) {
		return;
	}

	if (newSize < size()) {
		while (size() > newSize) {
			pop_back();
		}
	}
	else {
		while (size() < newSize) {
			emplace_back();
		}
	}
}

template<typename T>
inline bool szg::SparseSet<T>::is_valid_id(i64 id) const {
	return id >= 0 && id < id_space_size() && // idが有効な範囲内であることを確認
		indexes[id] < size(); // 対応するidxが有効か
}

template<typename T>
inline std::optional<i64> szg::SparseSet<T>::id_by_index(i64 idx) const {
	if (idx < 0 || idx >= size()) {
		return std::nullopt;
	}
	return ids[idx];
}

template<typename T>
inline void szg::SparseSet<T>::swap_ids(i64 lid, i64 rid) {
	if (!is_valid_id(lid) || !is_valid_id(rid)) {
		szgError("Invalid id: {} or {}", lid, rid);
		return;
	}
	if (lid == rid) {
		return;
	}

	i64 lidx = indexes[lid];
	i64 ridx = indexes[rid];

	std::swap(data[lidx], data[ridx]);
	std::swap(ids[lidx], ids[ridx]);

	indexes[lid] = ridx;
	indexes[rid] = lidx;
}

template<typename T>
inline void szg::SparseSet<T>::swap_indexes(i64 lidx, i64 ridx) {
	if (lidx < 0 || lidx >= size() || ridx < 0 || ridx >= size()) {
		szgError("Invalid index: {} or {}", lidx, ridx);
		return;
	}
	if (lidx == ridx) {
		return;
	}

	i64 lid = ids[lidx];
	i64 rid = ids[ridx];

	std::swap(data[lidx], data[ridx]);
	std::swap(ids[lidx], ids[ridx]);

	indexes[lid] = ridx;
	indexes[rid] = lidx;

}

template<typename T>
inline std::vector<T>::iterator szg::SparseSet<T>::begin() {
	return data.begin();
}

template<typename T>
inline std::vector<T>::iterator szg::SparseSet<T>::end() {
	return data.end();
}

template<typename T>
inline std::vector<T>::const_iterator szg::SparseSet<T>::begin() const {
	return data.cbegin();
}

template<typename T>
inline std::vector<T>::const_iterator szg::SparseSet<T>::end() const {
	return data.cend();
}

template<typename T>
inline std::vector<T>::const_iterator szg::SparseSet<T>::cbegin() const {
	return data.cbegin();
}

template<typename T>
inline std::vector<T>::const_iterator szg::SparseSet<T>::cend() const {
	return data.cend();
}

template<typename T>
inline std::vector<T>::reverse_iterator szg::SparseSet<T>::rbegin() {
	return data.rbegin();
}

template<typename T>
inline std::vector<T>::reverse_iterator szg::SparseSet<T>::rend() {
	return data.rend();
}

template<typename T>
inline std::vector<T>::const_reverse_iterator szg::SparseSet<T>::crbegin() const {
	return data.crbegin();
}

template<typename T>
inline std::vector<T>::const_reverse_iterator szg::SparseSet<T>::crend() const {
	return data.crend();
}

template<typename T>
inline i64 szg::SparseSet<T>::id_space_size() const {
	return static_cast<i64>(ids.size());
}
