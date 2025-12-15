#ifdef DEBUG_FEATURES_ENABLE

#include "EditorInspector.h"

using namespace szg;

#include <imgui.h>

#include "../Core/EditorSelectObject.h"
#include "../RemoteObject/IRemoteObject.h"

void EditorInspector::initialize() {
	isActive = true;
}

void EditorInspector::setup(Reference<EditorSelectObject> selectObject_) {
	selectObject = selectObject_;
}

void EditorInspector::draw() {
	if (!isActive) {
		return;
	}

	int flags = 0;
	ImGui::Begin("Inspector", &isActive, flags);

	update_focus();

	Reference<IRemoteObject> select = selectObject->get_item_mut().object;

	if (select) {
		select->draw_inspector();
	}

	ImGui::End();
}

#endif // DEBUG_FEATURES_ENABLE
