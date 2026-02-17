#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

#include "./InstanceBucket.h"
#include "Engine/Application/Logger.h"
#include "Engine/Module/World/WorldInstance/WorldInstance.h"

namespace szg {

class WorldRoot final {
public:
	WorldRoot();
	~WorldRoot();

	SZG_CLASS_MOVE_ONLY(WorldRoot)

public:
	void initialize();
	void setup(Reference<InstanceBucket> instanceBucket_);

	void update();

	void update_affine();

	void post_update();

	template<std::derived_from<szg::WorldInstance> T, typename ...Args>
	Reference<T> instantiate(Reference<WorldInstance> parent = nullptr, Args&&... args);

	void destroy(Reference<WorldInstance> instance);

	void delete_marked_destroy();

private:
	u64 nextInstanceId = 0;
	std::unordered_map<u64, std::unique_ptr<WorldInstance>> worldInstances;

	std::vector<u64> destroyInstanceId;

	Reference<InstanceBucket> instanceBucket;
};

}; // szg

template<std::derived_from<szg::WorldInstance> T, typename ...Args>
inline Reference<T> szg::WorldRoot::instantiate(Reference<szg::WorldInstance> parent, Args&&... args) {
	std::unique_ptr<T> instance = std::make_unique<T>(std::forward<Args>(args)...);
	Reference<T> result = instance;
	auto [_, emplaced] = worldInstances.try_emplace(nextInstanceId, std::move(instance));
	if (!emplaced) {
		szgWarning("Instantiate failed for an unknown reason. Type-\'{}\'", typeid(T).name());
	}

	result->setup_id(nextInstanceId);
	result->setup_world_root(this);
	result->reparent(parent, false);

	instanceBucket->register_instance(result);

	++nextInstanceId;

	return result;
}
