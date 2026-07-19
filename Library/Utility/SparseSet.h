#pragma once

#include <vector>

namespace szg {

template<typename T>
class SparseSet final {
public:
	SparseSet() = default;
	~SparseSet() = default;

	SparseSet(i64 size, const T& defaultValue = {});

public:
	const T& at(i64 id) const;

	T& at(i64 id);

	const T& at_unchecked(i64 id) const;

	T& at_unchecked(i64 id);
	
	i64 size() const;

	void clear();

	bool empty() const;

	template<typename... Args>
	i64 emplace_back(Args&&... args);

	void remove(i64 id);

public:
	std::vector<T>::iterator begin();
	std::vector<T>::iterator end();

	std::vector<T>::const_iterator cbegin() const;
	std::vector<T>::const_iterator cend() const;

	std::vector<T>::reverse_iterator rbegin();
	std::vector<T>::reverse_iterator rend();

	std::vector<T>::const_reverse_iterator crbegin() const;
	std::vector<T>::const_reverse_iterator crend() const;

private:
	bool is_valid_id(i64 id) const;

private:
	std::vector<T> data;
	std::vector<i64> indexes;
	std::vector<i64> ids;
};

}

#include "SparseSet.inl"
