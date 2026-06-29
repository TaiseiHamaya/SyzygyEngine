#include "ColliderGroupMap.h"

szg::ColliderGroupMap::ColliderGroupMap(const std::unordered_map<std::string, i32>& groupMap)
	: groupMap(groupMap) {
}

std::optional<i32> szg::ColliderGroupMap::try_find_group(const std::string& groupName) const {
	if (groupMap.contains(groupName)) {
		return groupMap.at(groupName);
	}

	return std::nullopt;
}
