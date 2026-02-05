#pragma once

#include <Library/Utility/Template/Reference.h>

#include "../RenderPath/RenderPath.h"

namespace szg {

class RenderTexture;
class NonLightingPixelPipeline;
class DirectionalLightingPipeline;
class PointLightingPipeline;

class LayerRenderSubtree {
public:
	void setup();

	void begin_nodes();
	void next_node();

private:
	RenderPath renderSubtree;
};

}; // szg
