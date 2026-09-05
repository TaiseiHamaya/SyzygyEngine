#pragma once

#include <optional>
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
	/// indexの末尾の要素を削除
	/// </summary>
	void pop_back();

	/// <summary>
	/// サイズの変更
	/// </summary>
	/// <param name="newSize"></param>
	void resize(i64 newSize);

	/// <summary>
	/// IDが有効かどうかチェック
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	bool is_valid_id(i64 id) const;

	/// <summary>
	/// indexからIDを取得
	/// </summary>
	/// <param name="idx"></param>
	/// <returns>範囲外の場合std::nullopt</returns>
	std::optional<i64> id_by_index(i64 idx) const;

	/// <summary>
	/// ID基準で要素を入れ替え
	/// </summary>
	/// <param name="lid"></param>
	/// <param name="rid"></param>
	void swap_ids(i64 lid, i64 rid);

	/// <summary>
	/// index基準で要素を入れ替え
	/// </summary>
	/// <param name="lidx"></param>
	/// <param name="ridx"></param>
	void swap_indexes(i64 lidx, i64 ridx);

public:
	std::vector<T>::iterator begin();
	std::vector<T>::iterator end();

	std::vector<T>::const_iterator begin() const;
	std::vector<T>::const_iterator end() const;

	std::vector<T>::const_iterator cbegin() const;
	std::vector<T>::const_iterator cend() const;

	std::vector<T>::reverse_iterator rbegin();
	std::vector<T>::reverse_iterator rend();

	std::vector<T>::const_reverse_iterator crbegin() const;
	std::vector<T>::const_reverse_iterator crend() const;

private:
	/// <summary>
	/// ID空間のサイズを取得
	/// </summary>
	/// <returns></returns>
	i64 id_space_size() const;

private:
	std::vector<T> data;
	std::vector<i64> indexes;
	std::vector<i64> ids;
};

} // namespace szg

#include "SparseSet.inl"
