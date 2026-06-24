#pragma once

#include <concepts>
#include <utility>

#include "Library/Utility/Tools/Hash.h"

#undef small
#undef big

/// <summary>
/// ソート済みペア
/// テンプレート引数制約 : 比較演算可能である
/// </summary>
/// <typeparam name="T"></typeparam>
template<std::totally_ordered T>
class SortedPair {
public:
	constexpr SortedPair() = default;
	~SortedPair() = default;

	SortedPair(const SortedPair&) = default;
	SortedPair& operator=(const SortedPair&) = default;
	SortedPair(SortedPair&&) = default;
	SortedPair& operator=(SortedPair&&) = default;

public:
	constexpr bool operator==(const SortedPair& rhs) const {
		return values.first == rhs.values.first && values.second == rhs.values.second;
	}
	constexpr bool operator!=(const SortedPair& rhs) const {
		return !(*this == rhs);
	}

public:
	constexpr SortedPair(const T& val1, const T& val2);

public:
	constexpr const T& small_imm() const { return values.first; };
	constexpr const T& big_imm() const { return values.second; };
	constexpr T& small_mut() { return values.first; };
	constexpr T& big_mut() { return values.second; };

private:
	std::pair<T, T> values;
};

template<std::totally_ordered T>
constexpr SortedPair<T>::SortedPair(const T& val1, const T& val2) :
	values(
		val1 <= val2 ?
		std::make_pair(val1, val2) :
		std::make_pair(val2, val1)
	) {
}

namespace std {

template<typename T>
struct hash<SortedPair<T>> {
public:
	size_t operator()(const SortedPair<T>& val) const {
		return eps::hash_vector<T>({ val.big_imm(), val.small_imm() });
	}
};

}
