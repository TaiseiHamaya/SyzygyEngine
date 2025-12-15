#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class IEditorWindow {
public:
	IEditorWindow() = default;
	virtual ~IEditorWindow() = default;

	SZG_CLASS_MOVE_ONLY(IEditorWindow)

public:
	virtual void draw() = 0;

	void draw_menu(string_literal name);

	bool is_active() const;
	void set_active(bool active);

	bool is_focus() const;

protected:
	void update_focus();

protected:
	bool isActive{ false };
	bool isFocus{ false };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
