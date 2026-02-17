#ifdef DEBUG_FEATURES_ENABLE

#include "EditorDeletedObjectPool.h"

using namespace szg;

void EditorDeletedObjectPool::delete_sequence(Reference<IRemoteObject> parent, Reference<IRemoteObject> object) {
	sequences.emplace_back(parent, object);
}

void EditorDeletedObjectPool::solution_sequence() {
	for (auto& [parent, object] : sequences) {
		deletedInstances[object] = parent->move_force(object);
	}
	sequences.clear();
}

std::unique_ptr<IRemoteObject> EditorDeletedObjectPool::get_deleted_force(Reference<IRemoteObject> search) {
	if (!search) {
		return nullptr;
	}
	if (deletedInstances.contains(search)) {
		std::unique_ptr<IRemoteObject> result = std::move(deletedInstances.at(search));
		deletedInstances.erase(search);
		return std::move(result);
	}
	return nullptr;
}

#endif // DEBUG_FEATURES_ENABLE
