#pragma once

#include <filesystem>

#include <Library/Utility/Tools/ConstructorMacro.h>
#include <Library/Utility/Template/bitflag.h>

#include "Engine/Runtime/Scene/World/InstanceBucket.h"
#include "Engine/Runtime/Scene/World/WorldRoot.h"
#include "Engine/Module/Render/WorldRenderCollection.h"
#include "Engine/Runtime/Collision/CollisionManager.h"

namespace szg {

enum class WorldState {
	Default = 0,
	PauseUpdate = 1 << 0,
	PauseDraw = 1 << 1,

	PauseAll = PauseUpdate | PauseDraw,
};

SZG_BITFLAG(WorldState);

class WorldCluster final {
public:
	WorldCluster() = default;
	~WorldCluster() = default;

	SZG_CLASS_MOVE_ONLY(WorldCluster)

public:
	void initialize();
	void setup(const std::filesystem::path& setupFile);

	void begin_frame();

	void update();

	void pre_draw();

	void end_frame();

public:
	WorldRoot& world_root_mut();

public:
	Reference<WorldRenderCollection> render_collection();

private:
	eps::bitflag<WorldState> state{ WorldState::Default };

	WorldRoot worldRoot;
	InstanceBucket instanceBucket;
	WorldRenderCollection worldRenderCollection;
	CollisionManager collisionManager;
};

}; // szg
