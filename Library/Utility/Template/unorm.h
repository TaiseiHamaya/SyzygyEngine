#pragma once

#include <compare>
#include <format>
#include <limits>

/// <summary>
/// unorm型
/// </summary>
class unorm {
public: // using
	using value_type = u32;

public: // static member value
	constexpr static value_type value_max = std::numeric_limits<value_type>::max();
	constexpr static float value_max_float = static_cast<float>(value_max);

public: // constructor/destructor
	constexpr unorm() noexcept = default;
	constexpr ~unorm() noexcept = default;

	constexpr unorm(const unorm&) noexcept = default;
	constexpr unorm& operator=(const unorm&) noexcept = default;
	constexpr unorm(unorm&&) noexcept = default;
	constexpr unorm& operator=(unorm&&) noexcept = default;

public: // operator
	constexpr operator float() const noexcept;

	constexpr auto operator<=>(const unorm& rhs) const noexcept = default;
	constexpr bool operator==(const unorm& rhs) const noexcept = default;

	constexpr unorm operator+(const unorm& rhs) const noexcept;
	constexpr unorm operator-(const unorm& rhs) const noexcept;
	constexpr unorm operator*(const unorm& rhs) const noexcept;
	constexpr unorm operator/(const unorm& rhs) const noexcept;

	constexpr unorm& operator+=(const unorm& rhs) noexcept;
	constexpr unorm& operator-=(const unorm& rhs) noexcept;
	constexpr unorm& operator*=(const unorm& rhs) noexcept;
	constexpr unorm& operator/=(const unorm& rhs) noexcept;

public: // member function
	constexpr value_type native() const noexcept;

	constexpr float to_float() const noexcept;

public:
	static constexpr unorm from_native(value_type rhs) noexcept;

	static constexpr unorm from_float(float rhs) noexcept;

private: // member value
	value_type value{ 0 };
};

template<>
struct std::formatter<unorm> {
	constexpr auto parse(std::format_parse_context& ctx);

	auto format(const unorm& u, std::format_context& ctx) const;
};

#include "unorm.inl"
