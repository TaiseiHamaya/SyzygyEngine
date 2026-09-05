#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <array>
#include <string>
#include <variant>

#include <Library/Math/ColorRGB.h>
#include <Library/Math/ColorRGBA.h>
#include <Library/Math/Vector2.h>
#include <Library/Math/Vector3.h>
#include <Library/Utility/Template/unorm.h>

namespace szg {

using ParameterVariant =
std::variant<std::string, i32, u32, r32, unorm, bool, Vector2, Vector3, ColorRGB, ColorRGBA>;

enum class ParameterType {
	String,
	I32,
	U32,
	R32,
	UNorm,
	Bool,
	Vector2,
	Vector3,
	ColorRGB,
	ColorRGBA,

	COUNT
};

constexpr i64 ParameterTypeCount = static_cast<i64>(ParameterType::COUNT);

inline constexpr std::array<string_literal, ParameterTypeCount> ParameterTypeNames = {
	"String",
	"I32",
	"U32",
	"R32",
	"UNorm",
	"Bool",
	"Vector2",
	"Vector3",
	"ColorRGB",
	"ColorRGBA"
};

inline const std::array<ParameterVariant, ParameterTypeCount> ParameterDefaultValues = {
	std::string(""),
	i32(0),
	u32(0),
	r32(0.0f),
	unorm(),
	bool(false),
	Vector2(),
	Vector3(),
	ColorRGB(),
	ColorRGBA()
};

struct Parameter {
	std::string name{ "NewValue" };
	ParameterType type;
	ParameterVariant value;
	bool isDeleted{ false };
};

} // namespace szg

#endif // DEBUG_FEATURES_ENABLE
