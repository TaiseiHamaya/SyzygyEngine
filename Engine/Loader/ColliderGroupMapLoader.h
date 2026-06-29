#pragma once

#include <string>

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Runtime/Collision/ColliderGroupMap.h"

namespace szg {

class ColliderGroupMapLoader final {
public:
	ColliderGroupMapLoader() = default;
	~ColliderGroupMapLoader() = default;

	SZG_CLASS_MOVE_ONLY(ColliderGroupMapLoader)

public:
	std::pair<i32, ColliderGroupMap> load(const std::string& sceneName);
};

} // namespace szg
