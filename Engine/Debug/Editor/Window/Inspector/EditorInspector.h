#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IEditorWindow.h"

#include <Library/Utility/Template/Reference.h>

namespace szg {

class EditorSelectObject;

class EditorInspector : public IEditorWindow {
public:
	void setup(Reference<EditorSelectObject> selectObject_);

	void draw() override;

private:
	Reference<EditorSelectObject> selectObject;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
