#ifdef DEBUG_FEATURES_ENABLE

#include "IRemoteObject.h"

using namespace szg;

void IRemoteObject::reparent(Reference<IRemoteObject> remoteObject) {
	parent = remoteObject;
}

Reference<const RemoteWorldObject> IRemoteObject::query_world() const {
	if (!parent) {
		return nullptr;
	}
	return parent->query_world();
}

Reference<IRemoteObject> IRemoteObject::get_parent() const {
	return parent;
}

void IRemoteObject::Setup(Reference<EditorSceneView> sceneView_) {
	IRemoteObject::sceneView = sceneView_;
}

#endif // DEBUG_FEATURES_ENABLE
