#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class RuntimeRegisteredSceneList final {
public:
	RuntimeRegisteredSceneList() = default;
	~RuntimeRegisteredSceneList() = default;

	SZG_CLASS_MOVE_ONLY(RuntimeRegisteredSceneList);

public:
	void setup(const std::string& lastSelectedSceneName);

	void combo_gui();

public:
	std::optional<std::string_view> runtime_initial_scene() const;

	std::optional<i32> runtime_initial_scene_index() const;

private:
	std::unordered_map<std::string, i32> sceneNameToIndexMap;
	std::string currentSelectedSceneName;
};

} // namespace szg

#endif // DEBUG_FEATURES_ENABLE
