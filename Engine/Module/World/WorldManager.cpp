#include "WorldManager.h"

using namespace szg;

#include <execution>

WorldManager::WorldManager() {
	worldInstances.resize(10);
}

WorldManager::~WorldManager() = default;

void WorldManager::update_matrix() {
	for (auto& depth : worldInstances) {
		std::for_each(
			std::execution::par, depth.begin(), depth.end(),
			[](Reference<WorldInstance> instance) {
			instance->fixed_update();
			instance->update_affine();
		});
	}
}

void WorldManager::reset_depth(Reference<WorldInstance> instance, u32 after) {
	worldInstances[instance->hierarchy_depth()].erase(instance);
	worldInstances[after].emplace(instance);
}

void WorldManager::erase(Reference<WorldInstance> instance) {
	worldInstances[instance->hierarchy_depth()].erase(instance);
}

std::vector<std::unordered_set<Reference<WorldInstance>>>& WorldManager::get_world_instances() {
	return worldInstances;
}

const std::vector<std::unordered_set<Reference<WorldInstance>>>& WorldManager::get_world_instances() const {
	return worldInstances;
}
