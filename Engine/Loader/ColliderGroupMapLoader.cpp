#include "ColliderGroupMapLoader.h"

#include "Engine/Assets/Json/JsonAsset.h"

using namespace szg;

std::pair<i32, ColliderGroupMap>  szg::ColliderGroupMapLoader::load(const std::string& sceneName) {
	JsonAsset json{ std::format("./Game/Core/Scene/{}/CollisionGroup.json", sceneName) };

	if (json.cget().is_null()) {
		return {};
	}

	std::unordered_map<std::string, i32> groupMap{};
	i32 i = 0;
	for (const nlohmann::json& group : json.cget().value("Groups", nlohmann::json::array())) {
		if (!group.is_string()) {
			continue;
		}

		std::string groupName = group.get<std::string>();
		szgInformation("Added collider group: {} with ID: {}", groupName, i);
		groupMap.emplace(groupName, i);

		++i;
	}

	return std::make_pair(i, ColliderGroupMap{ groupMap });
}
