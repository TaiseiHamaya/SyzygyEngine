#ifdef DEBUG_FEATURES_ENABLE

#include "CustomEditorManager.h"

#include "Engine/Debug/Editor/Window/IEditorWindow.h"

#include <imgui.h>

void szg::CustomEditorManager::setup_window_active(u32 windowStateBit) {
	// custom editorの場合は31bit目から参照
	for (i32 i = 0; i < static_cast<i32>(editorWindows.size()); ++i) {
		if (windowStateBit & (1 << (31 - i))) {
			editorWindows[i].window->set_active(true);
		}
	}
}

u32 szg::CustomEditorManager::get_window_state_bit() const {
	u32 windowStateBit{ 0 };
	for (i32 i = 0; i < static_cast<i32>(editorWindows.size()); ++i) {
		if (editorWindows[i].window->is_active()) {
			windowStateBit |= (1 << (31 - i));
		}
	}
	return windowStateBit;
}

void szg::CustomEditorManager::register_editor_window(string_literal name, std::unique_ptr<IEditorWindow> window) {
	editorWindows.emplace_back(name, std::move(window));
}

void szg::CustomEditorManager::draw_menu() {
	for (const auto& editorWindowInfo : editorWindows) {
		editorWindowInfo.window->draw_menu(editorWindowInfo.name);
	}
}

void szg::CustomEditorManager::draw_window() {
	for (const auto& editorWindowInfo : editorWindows) {
		if (editorWindowInfo.window->is_active()) {
			bool isOpen = editorWindowInfo.window->is_active();
			ImGui::Begin(editorWindowInfo.name, &isOpen);
			editorWindowInfo.window->set_active(isOpen);
			editorWindowInfo.window->draw();
			ImGui::End();
		}
	}
}

#endif // DEBUG_FEATURES_ENABLE
