#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "./SceneAssetCollection.h"
#include "Engine/Runtime/SceneScript/SceneScriptManager.h"
#include "Engine/Runtime/Scene/World/WorldCluster.h"
#include "Engine/Module/Render/RenderDAG.h"

namespace szg {

class Scene {
#ifdef DEBUG_FEATURES_ENABLE
	friend class RemoteSceneObject;
	friend class EditorMain;
#endif // DEBUG_FEATURES_ENABLE

public:
	Scene() = default;
	virtual ~Scene() = default;

	SZG_CLASS_MOVE_ONLY(Scene)

public:
	void load_assets();

	virtual void custom_load_asset();

	void initialize();

	void setup();

	virtual void custom_setup() {};

	void begin_frame();

	void update();

	void pre_draw();

	void draw() const;

	void end_frame();

	void finalize();

public:
	i64 world_size() const noexcept;
	Reference<WorldCluster> world_mut(i64 index) noexcept;

	i64 script_size() const noexcept;
	Reference<ISceneScript> script_mut(i64 index) noexcept;

	void set_name(const std::string& name);
	std::string_view name() const noexcept;

protected:
	std::string sceneName;
	SceneScriptManager sceneScriptManager;

private:
	std::vector<std::unique_ptr<WorldCluster>> worlds;
	RenderDAG renderDAG;
	SceneAssetCollection assetCollection;
};

}; // szg
