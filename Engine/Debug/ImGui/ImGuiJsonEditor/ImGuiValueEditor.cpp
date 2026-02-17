#ifdef DEBUG_FEATURES_ENABLE

#include "ImGuiValueEditor.h"

using namespace szg;

void ImGuiValueEditor::show_imgui() {
	for (auto& function : functions) {
		function.imguiFunc();
	}
}

void ImGuiValueEditor::save_all() const {
	for (const auto& function : functions) {
		function.saveFunc();
	}
}

#endif // _DEBUG
