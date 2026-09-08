#pragma once

#include "Matrix.h"

#include <initializer_list>

class Matrix3x3;

// ------------------3x3の特殊行列を継承定義------------------
class Matrix4x4 final {
public:
	static constexpr size_t ROW = 4, COLUMN = 4;

public: // コピー、ムーブはデフォルト定義
	constexpr Matrix4x4() noexcept = default;
	constexpr Matrix4x4(const Matrix4x4& rhs) noexcept = default;
	constexpr Matrix4x4(Matrix4x4&& rhs) noexcept = default;
	constexpr Matrix4x4& operator=(const Matrix4x4&) noexcept = default;
	constexpr Matrix4x4& operator=(Matrix4x4&&) noexcept = default;

public: // その他コンストラクタ
	/// <summary>
	/// Matrix<4, 4>から生成
	/// </summary>
	/// <param name="rhs"></param>
	constexpr Matrix4x4(const Matrix<ROW, COLUMN>& rhs);
	/// <summary>
	/// initializer_listから生成
	/// </summary>
	/// <param name="init"></param>
	constexpr Matrix4x4(const std::initializer_list<std::initializer_list<r32>>& init);

public:
	// ------------------演算子オーバーロード------------------
	constexpr const std::array<r32, COLUMN>& operator[](std::size_t index) const;
	constexpr std::array<r32, COLUMN>& operator[](std::size_t index);

	constexpr const Matrix4x4 operator+(const Matrix4x4& opr) const;
	constexpr const Matrix4x4 operator-(const Matrix4x4& opr) const;
	constexpr const Matrix4x4 operator*(const Matrix4x4& opr) const;
	constexpr Matrix4x4& operator+=(const Matrix4x4& opr);
	constexpr Matrix4x4& operator*=(const Matrix4x4& opr);

	constexpr bool operator==(const Matrix4x4& rhs) const noexcept;
	constexpr bool operator!=(const Matrix4x4& rhs) const noexcept;

public: // static関数
	/// <summary>
	/// 定数倍
	/// </summary>
	/// <param name="matrix">元Matrix</param>
	/// <param name="times">倍数</param>
	/// <returns>計算値</returns>
	static constexpr Matrix4x4 Multiply(const Matrix4x4& matrix, const r32& times);

	/// <summary>
	/// 逆行列計算
	/// </summary>
	/// <param name="matrix">計算元行列</param>
	/// <returns>変換後行列</returns>
	static const Matrix4x4 Inverse(const Matrix4x4& matrix);

	/// <summary>
	/// 転置行列
	/// </summary>
	/// <param name="matrix">計算元行列</param>
	/// <returns>変換後行列</returns>
	static constexpr Matrix4x4 Transpose(const Matrix4x4& matrix);

	/// <summary>
	/// 3x3行列を4x4に変換(埋める値は全て0)
	/// </summary>
	/// <param name="matrix">3x3行列</param>
	/// <returns>変換後行列</returns>
	static constexpr Matrix4x4 Convert3x3(const Matrix3x3& matrix);

public: // メンバ関数
	/// <summary>
	/// 逆行列計算
	/// </summary>
	/// <returns>変換後行列</returns>
	const Matrix4x4 inverse() const;

	/// <summary>
	/// 転置行列
	/// </summary>
	/// <returns>変換後行列</returns>
	constexpr const Matrix4x4 transpose() const;

private:// has継承
	Matrix<ROW, COLUMN> _matrix;
};

constexpr Matrix4x4::Matrix4x4(const Matrix<4, 4>& rhs) {
	_matrix = rhs;
}

constexpr Matrix4x4::Matrix4x4(const std::initializer_list<std::initializer_list<r32>>& init) {
	for (i32 i = 0; i < ROW; ++i) {
		for (i32 j = 0; j < COLUMN; ++j) {
			_matrix[i][j] = *((init.begin() + i)->begin() + j);
		}
	}
}

constexpr const std::array<r32, Matrix4x4::COLUMN>& Matrix4x4::operator[](std::size_t index) const {
	return _matrix[index];
}

constexpr std::array<r32, Matrix4x4::COLUMN>& Matrix4x4::operator[](std::size_t index) {
	return _matrix[index];
}

constexpr const Matrix4x4 Matrix4x4::operator+(const Matrix4x4& opr) const {
	return (_matrix + opr._matrix);
}

constexpr const Matrix4x4 Matrix4x4::operator-(const Matrix4x4& opr) const {
	return (_matrix - opr._matrix);
}

constexpr const Matrix4x4 Matrix4x4::operator*(const Matrix4x4& opr) const {
	return (_matrix * opr._matrix);
}

constexpr Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& opr) {
	*this = (_matrix + opr._matrix);
	return *this;
}

constexpr Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& opr) {
	*this = (_matrix * opr._matrix);
	return *this;
}

constexpr bool Matrix4x4::operator==(const Matrix4x4& rhs) const noexcept {
	return _matrix == rhs._matrix;
}

constexpr bool Matrix4x4::operator!=(const Matrix4x4& rhs) const noexcept {
	return !(*this == rhs);
}

constexpr Matrix4x4 Matrix4x4::Multiply(const Matrix4x4& matrix, const r32& times) {
	return Matrix<4, 4>::Multiply(matrix._matrix, times);
}

constexpr Matrix4x4 Matrix4x4::Transpose(const Matrix4x4& matrix) {
	return matrix.transpose();
}

#include "Matrix3x3.h"
constexpr Matrix4x4 Matrix4x4::Convert3x3(const Matrix3x3& matrix) {
	Matrix4x4 result{};
	for (u32 r = 0; r < 3; ++r) {
		for (u32 c = 0; c < 3; ++c) {
			result[r][c] = matrix[r][c];
		}
	}
	return result;
}

constexpr const Matrix4x4 Matrix4x4::transpose() const {
	return _matrix.transpose();
}

namespace CMatrix4x4 {

inline constexpr Matrix4x4 IDENTITY = {
	{{1,0,0,0},
	{0,1,0,0},
	{0,0,1,0},
	{0,0,0,1}}
};

inline constexpr Matrix4x4 ZERO = {
	{{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0}}
};

}