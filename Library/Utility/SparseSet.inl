#pragma once

#include "SparseSet.h"

#include <numeric>

#include "Engine/Application/Logger.h"

template<typename T>
inline szg::SparseSet<T>::SparseSet(i64 size, const T& defaultValue) {
	data.resize(size, defaultValue);
	indexes.reserve(size);
	ids.reserve(size);

	std::iota(indexes.begin(), indexes.end(), 0);
	std::iota(ids.begin(), ids.end(), 0);
}

template<typename T>
inline Reference<const T> szg::SparseSet<T>::at(i64 id) const {
	if (!is_valid_id(id)) {
		szgError("SparseSet::at() - Invalid id: {}", id);
		return nullptr;
	}
	return data[indexes[id]];
}

template<typename T>
inline Reference<T> szg::SparseSet<T>::at_mut(i64 id) {
	if (!is_valid_id(id)) {
		szgError("SparseSet::at_mut() - Invalid id: {}", id);
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
	data.emplace_back(std::forward<Args>(args)...);

	i64 idx = size();
	if (data.size() > indexes.size()) {
		indexes.emplace_back(idx);
		ids.emplace_back(idx);
	}

	return ids[idx - 1];
}

template<typename T>
inline void szg::SparseSet<T>::remove(i64 removeId) {
	if (!is_valid_id(removeId)) {
		return;
	}

	i64 backIdx = size() - 1;

	std::swap(data[removeId], data[backIdx]);
	std::swap(ids[removeId], ids[backIdx]);

	indexes[ids[backIdx]] = backIdx;
	indexes[ids[removeId]] = removeId;

	data.pop_back();
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
inline bool szg::SparseSet<T>::is_valid_id(i64 id) const {
	return id >= 0 && id < size();
}
