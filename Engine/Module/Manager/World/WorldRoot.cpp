#include "WorldRoot.h"

using namespace szg;

WorldRoot::WorldRoot() = default;
WorldRoot::~WorldRoot() = default;

void WorldRoot::initialize() {
}

void WorldRoot::setup(Reference<InstanceBucket> instanceBucket_) {
	instanceBucket = instanceBucket_;
}

void WorldRoot::update() {
	for (auto& [_, instance] : worldInstances) {
		instance->update();
	}
}

void WorldRoot::update_affine() {
	for (auto& [_, instance] : worldInstances) {
		instance->fixed_update();
		instance->update_affine();
	}
}

void WorldRoot::post_update() {
	for (auto& [_, instance] : worldInstances) {
		instance->late_update();
	}
}

void WorldRoot::destroy(Reference<WorldInstance> instance) {
	if (!instance) {
		return;
	}
	instance->mark_destroy();
	destroyInstanceId.emplace_back(instance->instance_id());
}

void WorldRoot::delete_marked_destroy() {
	for (auto& id : destroyInstanceId) {
		worldInstances.erase(id);
	}
	destroyInstanceId.clear();
}
