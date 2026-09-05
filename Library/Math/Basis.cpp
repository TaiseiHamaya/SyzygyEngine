#include "Basis.h"

#include <cmath>

#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"

#include "Engine/Application/Logger.h"

#define cofac(row1, col1, row2, col2) \
	(rows[row1][col1] * rows[row2][col2] - rows[row1][col2] * rows[row2][col1])

void Basis::invert() {
	r32 co[3] = {
		cofac(1, 1, 2, 2), cofac(1, 2, 2, 0), cofac(1, 0, 2, 1)
	};
	r32 det = rows[0][0] * co[0] +
		rows[0][1] * co[1] +
		rows[0][2] * co[2];
	r32 s = 1.0f / det;

	szgWarningIf(det == 0.0f, "Basis::invert(inverse) is called. But determinant was zero");

	*this = {
		Vector3(co[0] * s, cofac(0, 2, 2, 1) * s, cofac(0, 1, 1, 2) * s),
		Vector3(co[1] * s, cofac(0, 0, 2, 2) * s, cofac(0, 2, 1, 0) * s),
		Vector3(co[2] * s, cofac(0, 1, 2, 0) * s, cofac(0, 0, 1, 1) * s)
	};
}

void Basis::transpose() {
	std::swap(rows[0][1], rows[1][0]);
	std::swap(rows[0][2], rows[2][0]);
	std::swap(rows[1][2], rows[2][1]);
}

Basis Basis::inverse() const {
	Basis inv = *this;
	inv.invert();
	return inv;
}

Basis Basis::transposed() const {
	Basis tr = *this;
	tr.transpose();
	return tr;
}

Basis Basis::orthonormalize() const {
	Basis result;
	result.rows[0] = rows[0].normalize_safe();
	result.rows[1] = rows[1].normalize_safe();
	result.rows[2] = rows[2].normalize_safe();

	return result;
}

Matrix3x3 Basis::to_matrix() const {
	Matrix3x3 result;
	for (i32 i = 0; i < 3; ++i) {
		std::memcpy(result[i].data(), &rows[i], sizeof(r32) * 3);
	}
	return result;
}

Matrix4x4 Basis::to_matrix4x4() const {
	Matrix4x4 result;
	for (i32 i = 0; i < 3; ++i) {
		std::memcpy(result[i].data(), &rows[i], sizeof(r32) * 3);
	}
	return result;
}

void Basis::scale_rotate(const Vector3& scale, const Quaternion& rotate) {
	*this = rotate.to_basis();
	rows[0] *= scale.x;
	rows[1] *= scale.y;
	rows[2] *= scale.z;
}

Vector3 Basis::to_scale() const {
	return {
		rows[0].length(),
		rows[1].length(),
		rows[2].length()
	};
}

Quaternion Basis::to_quaternion() const {
	// 理論 : https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	// Code : https://github.com/godotengine/godot/blob/master/core/math/basis.cpp (GodotEngine Basis::get_quaternion()関数より)

	Basis norm = orthonormalize();

	Vector3 xyz;
	r32 w;
	r32 trace = norm[0][0] + norm[1][1] + norm[2][2];
	if (trace > 0.0f) {
		r32 inv4w = 0.5f / std::sqrt(trace + 1.0f);
		xyz[0] = (norm[1][2] - norm[2][1]) * inv4w;
		xyz[1] = (norm[2][0] - norm[0][2]) * inv4w;
		xyz[2] = (norm[0][1] - norm[1][0]) * inv4w;
		w = (trace + 1.0f) * inv4w;
	}
	else {
		i32 i = norm[0][0] < norm[1][1]
			? (norm[1][1] < norm[2][2] ? 2 : 1)
			: (norm[0][0] < norm[2][2] ? 2 : 0);
		i32 j = (i + 1) % 3;
		i32 k = (i + 2) % 3;

		r32 fourSqr = norm[i][i] - norm[j][j] - norm[k][k] + 1.0f;
		r32 inv4 = 0.5f / std::sqrt(fourSqr);

		xyz[i] = inv4 * fourSqr;
		xyz[j] = (norm[i][j] + norm[j][i]) * inv4;
		xyz[k] = (norm[i][k] + norm[k][i]) * inv4;
		w = (norm[j][k] - norm[k][j]) * inv4;
	}

	return Quaternion{ xyz,w };
}

Basis Basis::FromScale(const Vector3& scale) {
	Basis result;
	result[0][0] *= scale.x;
	result[1][1] *= scale.y;
	result[2][2] *= scale.z;

	return result;
}

Basis Basis::FromRotation(const Quaternion& rotation) {
	Basis result;
	result = rotation.to_basis();

	return result;
}
