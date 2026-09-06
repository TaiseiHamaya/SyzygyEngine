#include "WorldRoot.h"

#include "Engine/Module/World/Particle/EmitterInstance.h"
#include "Engine/Runtime/Particle/ParticlePool.h"
#include "Engine/Runtime/Particle/ParticleUpdaterCollection.h"

using namespace szg;

WorldRoot::WorldRoot() = default;
WorldRoot::~WorldRoot() = default;

void WorldRoot::initialize() {
}

void WorldRoot::setup(Reference<InstanceBucket> instanceBucket_, Reference<ParticleUpdaterCollection> particleUpdaters_) {
	instanceBucket = instanceBucket_;
	particleUpdaters = particleUpdaters_;
}

void WorldRoot::update() {
	for (auto& [_, instance] : worldInstances) {
		instance->update();
	}
	update_particle_pools();
}

void WorldRoot::update_affine() {
	for (auto& [_, instance] : worldInstances) {
		if (instance->hierarchy_depth() == 0) {
			instance->update_affine();
		}
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
		particlePools.erase(id);
		worldInstances.erase(id);
	}
	destroyInstanceId.clear();
}

Reference<ParticlePool> WorldRoot::create_particle_pool(Reference<EmitterInstance> owner, u32 capacity, ParticleOverflowPolicy policy) {
	if (!owner) {
		return nullptr;
	}
	std::unique_ptr<ParticlePool> pool = std::make_unique<ParticlePool>();
	pool->setup(particleUpdaters, owner, capacity, policy);
	Reference<ParticlePool> result = pool.get();
	particlePools.try_emplace(owner->instance_id(), std::move(pool));
	return result;
}

void WorldRoot::update_particle_pools() {
	if (!particleUpdaters) {
		return;
	}
	for (auto& [_, pool] : particlePools) {
		particleUpdaters->update_pool(pool.get());
		pool->reclaim();
	}
}
