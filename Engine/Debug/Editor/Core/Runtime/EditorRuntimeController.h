#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <vector>

#include <Library/Utility/Template/bitflag.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

#include "./RuntimeRegisteredSceneList.h"
#include "Engine/Runtime/Scene/World/WorldCluster.h"

namespace szg {

class EditorRuntimeController {
public:
	enum class State {
		Play,
		Pause,
		Stop,
	};

public:
	EditorRuntimeController() = default;
	~EditorRuntimeController() = default;

	SZG_CLASS_MOVE_ONLY(EditorRuntimeController);

public:
	void setup(const std::string& lastSelectedSceneName);

	void update();

	void control_gui(r32 menuHeight);

private:
	void start_runtime();
	void stop_runtime();
	void pause_runtime();

	void save_world_states();
	void restore_world_states();

public:
	std::optional<std::string_view> runtime_initial_scene() const;

private:
	State currentState{ State::Play };
	std::bitset<2> isStepFrameState;

	std::vector<eps::bitflag<WorldState>> worldStates;
	std::vector<bool> sceneScripts;

	RuntimeRegisteredSceneList registeredSceneList;
};

} // namespace szg

#endif // DEBUG_FEATURES_ENABLE
