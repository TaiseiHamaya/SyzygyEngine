#ifdef DEBUG_FEATURES_ENABLE

#include "EditorSelectObject.h"

#include "../RemoteObject/IRemoteObject.h"

using namespace szg;

void EditorSelectObject::set_item(Reference<IRemoteObject> object_, EditorSelectObjectBody::TransformData transform_) {
	select.object = object_;
	select.transformData = transform_;
}

EditorSelectObjectBody& EditorSelectObject::get_item_mut() {
	return select;
}

bool EditorSelectObject::is_selected(Reference<const IRemoteObject> rhs) const {
	return select.object == rhs;
}

#endif // DEBUG_FEATURES_ENABLE
