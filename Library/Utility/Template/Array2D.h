#pragma once

#include <array>

/// <summary>
/// クラスTでの2次元配列コンテナクラス
/// </summary>
/// <typeparam name="T">使用するクラス名</typeparam>
/// <typeparam name="ROW">行数</typeparam>
/// <typeparam name="COLUMN">列数</typeparam>
template<typename T, size_t ROW, size_t COLUMN>
class Array2D { // クラスTの例外表出場権をそのまま適用する
public:
	constexpr Array2D() noexcept(std::is_nothrow_default_constructible<T>::value);
	~Array2D() noexcept(std::is_nothrow_destructible<T>::value) = default;

public:
	constexpr Array2D(const Array2D&) noexcept(std::is_nothrow_copy_constructible<T>::value) = default;
	constexpr Array2D(Array2D&&) noexcept(std::is_nothrow_move_constructible<T>::value) = default;

	constexpr Array2D& operator=(const Array2D&) noexcept(std::is_nothrow_copy_assignable<T>::value) = default;
	constexpr Array2D& operator=(Array2D&&) noexcept(std::is_nothrow_move_assignable<T>::value) = default;

public:
	constexpr size_t row() const noexcept { return ROW; }
	constexpr size_t column() const noexcept { return COLUMN; }
	constexpr std::array<T, COLUMN>& operator[](size_t index) noexcept(false) { return data_[index]; }
	constexpr const std::array<T, COLUMN>& operator[](size_t index) const noexcept(false) { return data_[index]; }
	constexpr bool operator==(const Array2D<T, ROW, COLUMN>& rhs) const noexcept { return data_ == rhs.data_; }
	constexpr bool operator!=(const Array2D<T, ROW, COLUMN>& rhs) const noexcept { return !(*this == rhs); }

private:
	std::array<std::array<T, COLUMN>, ROW> data_;
};

template<typename T, size_t ROW, size_t COLUMN>
constexpr Array2D<T, ROW, COLUMN>::Array2D() noexcept(std::is_nothrow_default_constructible<T>::value) {
	data_ = { 0 };
}
