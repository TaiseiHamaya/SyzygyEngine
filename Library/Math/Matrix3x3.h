#pragma once

#include "Matrix.h"

#include <initializer_list>

// ------------------3x3の特殊行列を継承定義------------------
class Matrix3x3 final {
public: // コンストラクタ
	constexpr Matrix3x3() noexcept = default;
	constexpr Matrix3x3(const Matrix3x3& rhs) noexcept = default;
	constexpr Matrix3x3(Matrix3x3&& rhs) noexcept = default;
	constexpr Matrix3x3(const Matrix<3, 3>&& rhs) noexcept;
	constexpr Matrix3x3(const std::initializer_list<std::initializer_list<r32>>& init);

public:
	// ------------------演算子オーバーロード------------------
	constexpr Matrix3x3& operator=(const Matrix3x3&) noexcept = default;
	constexpr Matrix3x3& operator=(Matrix3x3&&) noexcept = default;

	constexpr const std::array<r32, 3>& operator[](std::size_t index) const;
	constexpr std::array<r32, 3>& operator[](std::size_t index);

	constexpr const Matrix3x3 operator+(const Matrix3x3& opr) const;
	constexpr const Matrix3x3 operator*(const Matrix3x3& opr) const;
	constexpr Matrix3x3& operator+=(const Matrix3x3& opr);
	constexpr Matrix3x3& operator*=(const Matrix3x3& opr);

public: // static関数
	/// <summary>
	/// 定数倍
	/// </summary>
	/// <param name="matrix">元Matrix</param>
	/// <param name="times">倍数</param>
	/// <returns>計算値</returns>
	static constexpr Matrix3x3 Multiply(const Matrix3x3& matrix, const r32& times);

	/// <summary>
	/// 逆行列計算
	/// </summary>
	/// <param name="matrix">計算元行列</param>
	/// <returns>変換後行列</returns>
	static Matrix3x3 Inverse(const Matrix3x3& matrix);

	/// <summary>
	/// 転置行列
	/// </summary>
	/// <param name="matrix">計算元行列</param>
	/// <returns>変換後行列</returns>
	static constexpr Matrix3x3 Transpose(const Matrix3x3& matrix);

public: // メンバ関数
	/// <summary>
	/// 逆行列計算
	/// </summary>
	/// <returns>変換後行列</returns>
	const Matrix3x3 inverse() const;

	/// <summary>
	/// 転置行列
	/// </summary>
	/// <returns>変換後行列</returns>
	constexpr const Matrix3x3 transpose() const;

private:// has継承
	Matrix<3, 3> matrix; // 実体
};

constexpr Matrix3x3::Matrix3x3(const Matrix<3, 3>&& rhs) noexcept {
	matrix = rhs;
}

constexpr Matrix3x3::Matrix3x3(const std::initializer_list<std::initializer_list<r32>>& init) {
	for (i32 i = 0; i < 3; ++i) {
		for (i32 j = 0; j < 3; ++j) {
			matrix[i][j] = *((init.begin() + i)->begin() + j);
		}
	}
}

constexpr const std::array<r32, 3>& Matrix3x3::operator[](std::size_t index) const {
	return matrix[index];
}

constexpr std::array<r32, 3>& Matrix3x3::operator[](std::size_t index) {
	return matrix[index];
}

constexpr const Matrix3x3 Matrix3x3::operator+(const Matrix3x3& opr) const {
	return (matrix + opr.matrix);
}

constexpr const Matrix3x3 Matrix3x3::operator*(const Matrix3x3& opr) const {
	return (matrix * opr.matrix);
}

constexpr Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& opr) {
	*this = (matrix + opr.matrix);
	return *this;
}

constexpr Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& opr) {
	*this = (matrix * opr.matrix);
	return *this;
}

constexpr Matrix3x3 Matrix3x3::Multiply(const Matrix3x3& matrix, const r32& times) {
	return Matrix<3, 3>::Multiply(matrix.matrix, times);
}


constexpr Matrix3x3 Matrix3x3::Transpose(const Matrix3x3& matrix) {
	return matrix.transpose();
}

constexpr const Matrix3x3 Matrix3x3::transpose() const {
	return matrix.transpose();
}

namespace CMatrix3x3 {
constexpr Matrix3x3 IDENTITY = {
	{{1,0,0},
	{0,1,0},
	{0,0,1}}
};
}