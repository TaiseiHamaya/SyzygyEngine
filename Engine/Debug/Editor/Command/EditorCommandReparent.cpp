#ifdef DEBUG_FEATURES_ENABLE

#include "EditorCommandReparent.h"

#include "Engine/Application/Logger.h"

using namespace szg;

EditorCommandReparent::EditorCommandReparent(Reference<IRemoteObject> target_, Reference<IRemoteObject> recent, Reference<IRemoteObject> next) {
	if(!target_) {
		szgWarning("Target is nullptr.");
		return;
	}
	if(!recent) {
		szgWarning("Recent parent is nullptr.");
		return;
	}
	if(!next) {
		szgWarning("Next parent is nullptr.");
		return;
	}
	target = target_;
	recentParent = recent;
	nextParent = next;
}

void EditorCommandReparent::execute() {
	auto instance = recentParent->move_force(target);
	if (!instance) {
		szgWarning("Failed to move target to next parent.");
		return;
	}

	nextParent->add_child(std::move(instance));
	target->on_spawn();
}

void EditorCommandReparent::undo() {
	auto instance = nextParent->move_force(target);
	if (!instance) {
		szgWarning("Failed to move target back to recent parent.");
		return;
	}

	recentParent->add_child(std::move(instance));
	target->on_spawn();
}

#endif // DEBUG_FEATURES_ENABLE
