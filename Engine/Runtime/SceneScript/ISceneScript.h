#pragma once

#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class ISceneScript {
public:
	ISceneScript() = default;
	virtual ~ISceneScript() = default;

	SZG_CLASS_MOVE_ONLY(ISceneScript)

public:
	virtual void finalize() {};

	virtual void prev_update() {};
	virtual void post_update() {};

public:
	bool is_paused() const noexcept;

	void set_pause(bool pause);

private:
	bool isPause{ false };
};

}; // szg
