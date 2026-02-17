#ifdef DEBUG_FEATURES_ENABLE

#include "EditorSelectCommand.h"

using namespace szg;

EditorSelectCommand::EditorSelectCommand(Reference<IRemoteObject> object, EditorSelectObjectBody::TransformData transform) {
	next.object = object;
	next.transformData = transform;
	prev = selectObject->get_item_mut();
}

void EditorSelectCommand::execute() {
	selectObject->set_item(next.object, next.transformData);
}

void EditorSelectCommand::undo() {
	selectObject->set_item(prev.object, prev.transformData);
}

void EditorSelectCommand::Setup(Reference<EditorSelectObject> selectObject_) {
	selectObject = selectObject_;
}

#endif // DEBUG_FEATURES_ENABLE
