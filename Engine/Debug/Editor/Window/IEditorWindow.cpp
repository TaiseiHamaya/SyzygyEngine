#ifdef DEBUG_FEATURES_ENABLE

#include "IEditorWindow.h"

#include <string>

#include <imgui.h>

using namespace szg;

void IEditorWindow::draw_menu(string_literal name) {
	std::string itemName = name;
	if (isActive) {
		itemName += "\ue5ca";
	}
	if (ImGui::MenuItem(itemName.c_str())) {
		isActive = true;
	}
}

bool IEditorWindow::is_active() const {
	return isActive;
}

void IEditorWindow::set_active(bool active) {
	isActive = active;
}

void szg::IEditorWindow::update_focus() {
	isFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
}

bool szg::IEditorWindow::is_focus() const {
	return isFocus;
}

#endif // DEBUG_FEATURES_ENABLE
