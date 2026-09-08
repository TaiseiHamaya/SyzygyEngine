#pragma once

#include "Basis.h"
#include "Vector3.h"

class Matrix4x4;
class Transform3D;

class Affine {
private:
	Basis basis;
	Vector3 origin;

public:
	Affine operator*(const Affine& rhs) const;
	Affine& operator*=(const Affine& rhs);

	friend constexpr Vector3 operator*(const Vector3& v, const Affine& affine);

public:
	static Affine FromTransform3D(const Transform3D& transform);
	static Affine FromSRT(const Vector3& scale, const Quaternion& rotation, const Vector3& translate);
	static Affine FromScale(const Vector3& scale);
	static Affine FromRotation(const Quaternion& rotation);
	static Affine FromTranslate(const Vector3& translate);
	static Affine FromMatrix(const Matrix4x4& matrix);

public:
	constexpr Vector3 transform(const Vector3& v) const;

	/// <summary>
	/// 自身の逆行列に変換(高速版)
	/// 正規直行の場合にのみ使用可能
	/// </summary>
	void invert_fast();

	/// <summary>
	/// 逆行列を返す(高速版)
	/// 正規直行の場合にのみ使用可能
	/// </summary>
	/// <returns></returns>
	Affine inverse_fast() const;

	/// <summary>
	/// 自身を逆行列に変換
	/// </summary>
	void invert();

	/// <summary>
	/// 逆行列を返す
	/// </summary>
	/// <returns></returns>
	Affine inverse() const;

public:
	constexpr const Vector3& get_origin() const { return origin; };
	constexpr const Basis& get_basis() const { return basis; };

public:
	Matrix4x4 to_matrix() const;
};

constexpr Vector3 operator*(const Vector3& v, const Affine& affine) {
	return v * affine.basis + affine.origin;
}

constexpr Vector3 Affine::transform(const Vector3& v) const {
	return v * *this;
}

namespace CAffine {

inline constexpr Affine IDENTITY{};

}
