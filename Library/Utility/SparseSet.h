#pragma once

#include <vector>

#include "Library/Utility/Template/Reference.h"
#include "Library/Utility/Tools/ConstructorMacro.h"

namespace szg {

/// <summary>
/// IDが疎かつ要素連続を保証するコンテナ
/// </summary>
/// <typeparam name="T"></typeparam>
template<typename T>
class SparseSet final {
public:
	SparseSet() = default;
	~SparseSet() = default;

	/// <summary>
	/// サイズ指定のコンストラクタ
	/// </summary>
	/// <param name="size"></param>
	/// <param name="defaultValue"></param>
	SparseSet(i64 size, const T& defaultValue = {});

	SZG_CLASS_DEFAULT(SparseSet<T>);

public:
	/// <summary>
	/// 要素の取得
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	Reference<const T> at(i64 id) const;

	/// <summary>
	/// 要素の取得
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	Reference<T> at_mut(i64 id);

	/// <summary>
	/// 要素の取得(境界チェックなし)
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	const T& at_unchecked(i64 id) const;

	/// <summary>
	/// 要素の取得(境界チェックなし)
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	T& at_mut_unchecked(i64 id);
	
	/// <summary>
	/// 現在の要素数
	/// </summary>
	/// <returns></returns>
	i64 size() const;

	/// <summary>
	/// 全要素の削除
	/// </summary>
	void clear();

	/// <summary>
	/// 要素が空かどうか
	/// </summary>
	/// <returns></returns>
	bool empty() const;

	/// <summary>
	/// 要素を追加
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="...args"></param>
	/// <returns></returns>
	template<typename... Args>
	i64 emplace_back(Args&&... args);

	/// <summary>
	/// 要素の削除
	/// </summary>
	/// <param name="id"></param>
	void remove(i64 id);

	/// <summary>
	/// 要素の入れ替え
	/// </summary>
	/// <param name="lhs"></param>
	/// <param name="rhs"></param>
	void swap_item(i64 lhs, i64 rhs);

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
	/// <summary>
	/// IDが有効かどうかチェック
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	bool is_valid_id(i64 id) const;

private:
	std::vector<T> data;
	std::vector<i64> indexes;
	std::vector<i64> ids;
};

} // namespace szg

#include "SparseSet.inl"
