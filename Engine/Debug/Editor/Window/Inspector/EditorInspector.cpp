#ifdef DEBUG_FEATURES_ENABLE

#include "EditorInspector.h"

#include <imgui.h>

#include "../../Core/EditorSelectObject.h"
#include "../../RemoteObject/IRemoteObject.h"

using namespace szg;

void EditorInspector::setup(Reference<EditorSelectObject> selectObject_) {
	selectObject = selectObject_;
}

void EditorInspector::draw() {
	if (!isActive) {
		return;
	}

	ImGui::Begin("Inspector", &isActive, windowFlags);

	update_focus();

	Reference<IRemoteObject> select = selectObject->get_item_mut().object;

	if (select) {
		select->draw_inspector();
	}

	ImGui::End();
}

#endif // DEBUG_FEATURES_ENABLE
