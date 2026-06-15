#include "InstanceBucket.h"

using namespace szg;

void InstanceBucket::reset() {
	camera.clear();
	staticMesh.clear();
	skinMesh.clear();
	directionalLightInstance.clear();
	pointLightInstance.clear();
	rect.clear();
	stringRect.clear();
}
