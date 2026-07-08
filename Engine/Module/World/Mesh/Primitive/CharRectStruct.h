#pragma once

#include <Library/Math/Vector2.h>

namespace szg {

struct GlyphRenderingData {
	Vector2 topLeft{};
	Vector2 size{};
	u32 glyphIndex{};
};

}; // szg
