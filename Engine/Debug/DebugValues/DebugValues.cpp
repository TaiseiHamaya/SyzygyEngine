#include "DebugValues.h"

using namespace szg;

DebugValues::DebugValues() {
	gridMesh = std::make_unique<StaticMeshInstance>("Grid.gltf");
}

DebugValues& DebugValues::GetInstance() {
	static DebugValues instance;
	return instance;
}

Reference<const StaticMeshInstance> DebugValues::GetGridInstance() {
	auto& instance = GetInstance();
	return instance.gridMesh;
}
