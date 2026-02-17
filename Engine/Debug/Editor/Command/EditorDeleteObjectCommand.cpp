#ifdef DEBUG_FEATURES_ENABLE

#include "EditorDeleteObjectCommand.h"

#include "../RemoteObject/EditorDeletedObjectPool.h"

using namespace szg;

EditorDeleteObjectCommand::EditorDeleteObjectCommand(Reference<IRemoteObject> object_) {
	object = object_;
	parent = object->get_parent();
}

void EditorDeleteObjectCommand::execute() {
	deletedPool->delete_sequence(parent, object);
	object->on_destroy();
}

void EditorDeleteObjectCommand::undo() {
	auto instance = deletedPool->get_deleted_force(object);
	Reference<IRemoteObject> tmp = instance;
	parent->add_child(std::move(instance));
	tmp->on_spawn();
}

void EditorDeleteObjectCommand::Setup(Reference<EditorDeletedObjectPool> deletedPool_) {
	deletedPool = deletedPool_;
}

#endif // DEBUG_FEATURES_ENABLE
