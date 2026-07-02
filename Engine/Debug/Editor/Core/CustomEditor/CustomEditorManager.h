#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <memory>
#include <vector>

#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class IEditorWindow;

class CustomEditorManager final {
public:
	CustomEditorManager() = default;
	~CustomEditorManager() = default;

	SZG_CLASS_MOVE_ONLY(CustomEditorManager)

public:
	struct EditorWindowInfo {
		string_literal name;
		std::unique_ptr<IEditorWindow> window;
	};

public:
	void setup_window_active(u32 windowStateBit);

	u32 get_window_state_bit() const;

	void register_editor_window(string_literal name, std::unique_ptr<IEditorWindow> window);

	void draw_menu();

	void draw_window();

private:
	std::vector<EditorWindowInfo> editorWindows;
};

} // namespace szg

#endif // DEBUG_FEATURES_ENABLE
