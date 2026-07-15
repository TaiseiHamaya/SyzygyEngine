#pragma once

#include <memory>
#include <vector>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class ISceneScript;

class SceneScriptManager final {
public:
	SceneScriptManager();
	~SceneScriptManager();

	SZG_CLASS_MOVE_ONLY(SceneScriptManager)

public:
	void register_script(std::unique_ptr<ISceneScript> script);

	void prev_update();
	void post_update();

	void finalize();

public:
	i64 size() const noexcept;
	Reference<ISceneScript> script_mut(i64 index) noexcept;

private:
	std::vector<std::unique_ptr<ISceneScript>> scripts;
};

}; // szg
