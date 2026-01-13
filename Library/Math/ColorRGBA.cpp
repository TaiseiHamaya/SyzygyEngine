#include "ColorRGBA.h"

//#include "Easing.h"

#include <cmath>

#ifdef DEBUG_FEATURES_ENABLE
#include <imgui.h>
#endif // _DEBUG

#ifdef DEBUG_FEATURES_ENABLE
void ColorRGBA::debug_gui(string_literal tag) noexcept(false) {
	ImGui::ColorEdit4(tag, &(this->red), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_InputRGB);
}
#endif // _DEBUG

ColorRGBA ColorRGBA::Lerp(const ColorRGBA& color1, const ColorRGBA& color2, r32 t) {
	return ColorRGBA{
		std::lerp(color1.red, color2.red, t),
		std::lerp(color1.green, color2.green, t),
		std::lerp(color1.blue, color2.blue, t),
		std::lerp(color1.alpha, color2.alpha, t)
	};
}

ColorRGBA ColorRGBA::LerpElement(const ColorRGBA& color1, const ColorRGBA& color2, const ColorRGBA& t) {
	return ColorRGBA{
		std::lerp(color1.red, color2.red, t.red),
		std::lerp(color1.green, color2.green, t.green),
		std::lerp(color1.blue, color2.blue, t.blue),
		std::lerp(color1.alpha, color2.alpha, t.alpha)
	};
}
