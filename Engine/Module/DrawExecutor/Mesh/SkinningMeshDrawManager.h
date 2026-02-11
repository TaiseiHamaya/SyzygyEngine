#pragma once

#include <string>

#include "../BaseDrawManager.h"
#include "./SkinningMeshDrawExecutor.h"
#include "Engine/Module/World/Mesh/SkinningMeshInstance.h"

namespace szg {

class SkinningMeshDrawManager final : public BaseDrawManager<SkinningMeshDrawExecutor> {
public:
	SkinningMeshDrawManager() = default;
	virtual ~SkinningMeshDrawManager() = default;

	SZG_CLASS_MOVE_ONLY(SkinningMeshDrawManager)

public:
	void make_instancing(u32 layer, const std::string& meshName, u32 maxInstance);
};

}; // szg
