#include "ISceneScript.h"

bool szg::ISceneScript::is_paused() const noexcept {
	return isPause;
}

void szg::ISceneScript::set_pause(bool pause) {
	isPause = pause;
}
