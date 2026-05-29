#pragma once

#include "unorm.h"

#include "Library/Utility/Tools/MathEPS.h"

/// ------------------------------
/// operator
/// ------------------------------

constexpr unorm::operator float() const noexcept {
	return to_float();
}

/// ------------------------------
/// arithmetic operator
/// ------------------------------

constexpr unorm unorm::operator+(const unorm& rhs) const noexcept {
	unorm result = *this;
	result += rhs;
	return result;
}

constexpr unorm unorm::operator-(const unorm& rhs) const noexcept {
	unorm result = *this;
	result -= rhs;
	return result;
}

constexpr unorm unorm::operator*(const unorm& rhs) const noexcept {
	unorm result = *this;
	result *= rhs;
	return result;
}

constexpr unorm unorm::operator/(const unorm& rhs) const noexcept {
	unorm result = *this;
	result /= rhs;
	return result;
}

constexpr unorm& unorm::operator+=(const unorm& rhs) noexcept {
	value_type temp = value + rhs.value;
	value = temp | -(temp < value);
	return *this;
}

constexpr unorm& unorm::operator-=(const unorm& rhs) noexcept {
	value_type temp = value - rhs.value;
	value = temp & -(value >= rhs.value);
	return *this;
}

constexpr unorm& unorm::operator*=(const unorm& rhs) noexcept {
	u64 temp = static_cast<u64>(value) * rhs.value;
	value = static_cast<value_type>((temp + (temp >> 32) + 1) >> 32);
	return *this;
}

constexpr unorm& unorm::operator/=(const unorm& rhs) noexcept {
	if (value >= rhs.value) {
		value = 0xFFFFFFFF;
		return *this;
	}
	if (rhs.value == 0) {
		value = 0xFFFFFFFF;
		return *this;
	}

	u64 numerator = (static_cast<u64>(value) << 32) - value;
	value = static_cast<value_type>(numerator / rhs.value);
	return *this;
}

/// ------------------------------
/// member function
/// ------------------------------

constexpr unorm::value_type unorm::native() const noexcept {
	return value;
}

constexpr float unorm::to_float() const noexcept {
	constexpr float value_nextafter_min = 1.0f / value_max;
	return value * value_nextafter_min;
}

constexpr unorm unorm::from_native(value_type rhs) noexcept {
	unorm result;
	result.value = rhs;
	return result;
}

constexpr unorm unorm::from_float(float rhs) noexcept {
	unorm result;
	rhs = eps::saturate(rhs);
	double temp = static_cast<double>(rhs) * value_max;
	result.value = static_cast<value_type>(temp + 0.5);
	return result;
}

constexpr auto std::formatter<unorm>::parse(std::format_parse_context& ctx) {
	return ctx.begin();
}

inline auto std::formatter<unorm>::format(const unorm& u, std::format_context& ctx) const {
	return std::format_to(ctx.out(), "{}({})", u.to_float(), u.native());
}
