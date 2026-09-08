#pragma once

#include <array>

#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"

namespace szg {

namespace EditorUtils {

inline constexpr std::array<string_literal, BLEND_MODE_COUNT> BLEND_MODE_LABELS = {
	"None",
	"Alpha",
	"Add",
	"Subtract",
	"Multiply",
	"Screen",
};

} // namespace EditorUtils

} // namespace szg
