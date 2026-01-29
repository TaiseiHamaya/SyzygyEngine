#pragma once

#include <memory>
#include <vector>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Module/Render/RenderTargetCollection/RenderTargetCollection.h"

namespace szg {

class IRenderNode;
class Scene;

class RenderDAG final {
public:
	RenderDAG();
	~RenderDAG() noexcept;

	SZG_CLASS_MOVE_ONLY(RenderDAG)

public:
	void setup(std::string_view sceneName, Reference<Scene> scene);

	void render_entry_point() const;

private:
	RenderTargetCollection renderTargetCollection;
	std::vector<std::unique_ptr<IRenderNode>> renderGraph;
};

}; // szg
