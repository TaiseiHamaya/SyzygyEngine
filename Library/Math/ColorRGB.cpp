#include "ColorRGB.h"


#ifdef DEBUG_FEATURES_ENABLE
#include <imgui.h>
#endif // _DEBUG

#ifdef DEBUG_FEATURES_ENABLE
void ColorRGB::debug_gui(string_literal tag) noexcept(false) {
	ImGui::ColorEdit3(tag, &(this->red), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_InputRGB);
}
#endif // _DEBUG
