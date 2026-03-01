#pragma once

#include "../RenderPath/RenderPath.h"

namespace szg {

class LayerRenderSubtree {
public:
	void setup();

	void begin_nodes();
	void next_node();

private:
	RenderPath renderSubtree;
};

}; // szg
