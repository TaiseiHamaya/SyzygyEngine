#pragma once

#include <vector>

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Module/DrawExecutor/LightingExecutor/DirectionalLightingExecutor.h"
#include "Engine/Module/DrawExecutor/LightingExecutor/PointLightingExecutor.h"
#include "Engine/Module/DrawExecutor/Mesh/Primitive/StringRectDrawManager.h"
#include "Engine/Module/DrawExecutor/Mesh/Primitive/Rect3dDrawManager.h"
#include "Engine/Module/DrawExecutor/Mesh/SkinningMeshDrawManager.h"
#include "Engine/Module/DrawExecutor/Mesh/StaticMeshDrawManager.h"
#include "Engine/Module/World/Light/DirectionalLight/DirectionalLightInstance.h"
#include "Engine/Module/World/Light/PointLight/PointLightInstance.h"
#include "RenderNode/WorldLayerRenderNode.h"

namespace szg {

class WorldLayerRenderNode;
class InstanceBucket;

class WorldRenderCollection final {
	friend class WorldLayerRenderNode;

public:
	WorldRenderCollection() = default;
	~WorldRenderCollection() = default;

	SZG_CLASS_MOVE_ONLY(WorldRenderCollection)

public:
	void initialize();
	void setup(u8 numLayer);

	void remove_marked_destroy();
	void collect_instantiated(Reference<InstanceBucket> instanceBucket);

	void transfer();

	Reference<CameraInstance> camera_at(u32 index) const;

private:
	u8 numLayer{ 0 };

	StaticMeshDrawManager staticMeshDrawManager;
	SkinningMeshDrawManager skinningMeshDrawManager;
	Rect3dDrawManager rect3dDrawManager;
	StringRectDrawManager stringRectDrawManager;
	std::vector<DirectionalLightingExecutor> directionalLightingExecutors;
	std::vector<PointLightingExecutor> pointLightingExecutors;

	std::vector<Reference<DirectionalLightInstance>> directionalLights;
	std::vector<Reference<PointLightInstance>> pointLights;

	std::vector<Reference<CameraInstance>> cameras;
};

}; // szg
