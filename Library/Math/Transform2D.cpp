#include "Transform2D.h"

#include <cmath>

#include "VectorConverter.h"
#include "Transform3D.h"

Transform2D::Transform2D() noexcept {
	scale = { 1, 1 };
	rotate = 0;
	translate = { 0, 0 };
}

Transform2D::Transform2D(const Vector2& scale_, r32 rotate_, const Vector2& translate_) noexcept {
	scale = scale_;
	set_rotate(rotate_);
	translate = translate_;
}

Transform2D::Transform2D(Vector2&& scale_, r32 rotate_, Vector2&& translate_) noexcept {
	scale = std::move(scale_);
	set_rotate(rotate_);
	translate = std::move(translate_);
}

void Transform2D::set_scale(const Vector2& scale_) noexcept {
	scale = scale_;
}

void Transform2D::set_rotate(r32 rotate_) noexcept {
	rotate = rotate_;
}

void Transform2D::set_translate(const Vector2& translate_) noexcept {
	translate = translate_;
}

void Transform2D::set_translate_x(r32 x) noexcept {
	translate.x = x;
}

void Transform2D::set_translate_y(r32 y) noexcept {
	translate.y = y;
}

Matrix3x3 Transform2D::get_matrix() const noexcept {
	return Transform2D::MakeAffineMatrix(scale, rotate, translate);
}

Matrix4x4 Transform2D::get_matrix4x4_transform() const noexcept {
	return Transform3D::MakeAffineMatrix(Converter::ToVector3(scale, 1.0f), Quaternion::EulerRadian(0, 0, rotate), Converter::ToVector3(translate, 0.0f));
}

Matrix4x4 Transform2D::get_matrix4x4_padding() const {
	return Matrix4x4::Convert3x3(get_matrix());
}

const Vector2& Transform2D::get_scale() const noexcept {
	return scale;
}

const r32& Transform2D::get_rotate() const noexcept {
	return rotate;
}

const Vector2& Transform2D::get_translate() const noexcept {
	return translate;
}

Vector2& Transform2D::get_scale() noexcept {
	return scale;
}

r32& Transform2D::get_rotate() noexcept {
	return rotate;
}

Vector2& Transform2D::get_translate() noexcept {
	return translate;
}

void Transform2D::plus_translate(const Vector2& plus) noexcept {
	translate += plus;
}

Matrix3x3 Transform2D::MakeRotateMatrix(const r32 theta) noexcept {
	return MakeRotateMatrix(std::sin(theta), std::cos(theta));
}

Matrix3x3 Transform2D::MakeAffineMatrix(const Vector2& scale, const r32 theta, const Vector2& translate) noexcept {
	return MakeAffineMatrix(scale, std::sin(theta), std::cos(theta), translate);
}

constexpr Matrix3x3 Transform2D::MakeAffineMatrix(const Vector2& scale, const r32 sinTheta, const r32 cosTheta, const Vector2& translate) noexcept {
	return { {
		{ scale.x * cosTheta, scale.x * sinTheta, 0 },
		{ -scale.y * sinTheta, scale.y * cosTheta, 0 },
		{ translate.x, translate.y, 1 }
		} };
}
