#ifdef DEBUG_FEATURES_ENABLE

#include "EditorCreateObjectCommand.h"

using namespace szg;

#include "../RemoteObject/EditorDeletedObjectPool.h"
#include "../Window/SceneView/EditorSceneView.h"

EditorCreateObjectCommand::EditorCreateObjectCommand(Reference<IRemoteObject> parent_, std::unique_ptr<IRemoteObject> generatedObjectTemp_) {
	parent = parent_;
	generatedObjectTemp = std::move(generatedObjectTemp_);
	object = generatedObjectTemp;
}

void EditorCreateObjectCommand::execute() {
	if (generatedObjectTemp) { // 初回
		// SceneViewに登録
		parent->add_child(std::move(generatedObjectTemp));
		object->setup();
	}
	else { // undo
		auto instance = deletedPool->get_deleted_force(object);
		parent->add_child(std::move(instance));
		object->on_spawn();
	}
}

void EditorCreateObjectCommand::undo() {
	deletedPool->delete_sequence(parent, object);
	object->on_destroy();
}

void EditorCreateObjectCommand::Setup(Reference<EditorDeletedObjectPool> deletedPool_) {
	deletedPool = deletedPool_;
}

#endif // DEBUG_FEATURES_ENABLE
