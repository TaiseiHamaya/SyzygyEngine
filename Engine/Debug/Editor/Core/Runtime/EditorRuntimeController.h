#pragma once

#include <vector>

#include <Library/Utility/Template/bitflag.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

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
	void update();

	void control_gui(r32 menuHight);

private:
	void start_runtime();
	void stop_runtime();
	void pause_runtime();

	void save_world_states();
	void restore_world_states();

private:
	State currentState{ State::Play };
	std::bitset<2> isStepFrameState;

	std::vector<eps::bitflag<WorldState>> worldStates;
	std::vector<bool> sceneScripts;
};

} // namespace szg
