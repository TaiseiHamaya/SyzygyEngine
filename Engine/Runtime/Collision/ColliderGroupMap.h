#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class ColliderGroupMap final {
public:
	ColliderGroupMap() = default;
	~ColliderGroupMap() = default;

	SZG_CLASS_MOVE_ONLY(ColliderGroupMap)

	ColliderGroupMap(const std::unordered_map<std::string, i32>& groupMap);

public:
	std::optional<i32> try_find_group(const std::string& groupName) const;

private:
	std::unordered_map<std::string, i32> groupMap;

};

} // namespace szg
