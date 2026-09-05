#include "RenderDAG.h"

#include <format>

#include "Engine/Assets/Json/JsonAsset.h"
#include "Engine/Loader/RenderPath/RenderNodeLoader.h"
#include "Engine/Module/Render/RenderNode/IRenderNode.h"
#include "Engine/Runtime/Scene/Scene.h"

using namespace szg;

RenderDAG::RenderDAG() = default;
RenderDAG::~RenderDAG() noexcept = default;

void RenderDAG::setup(std::string_view sceneName, Reference<Scene> scene) {
	JsonAsset json{ std::format("./Game/Core/Scene/{}/RenderPath.json", sceneName) };

	renderTargetCollection.clear();

	RenderNodeLoader loader;
	loader.setup(scene, renderTargetCollection);

	if (json.cget().is_null()) {
		szgWarning("RenderDAG::setup: Failed to load RenderPath.json: {}", sceneName);
		return;
	}

	renderGraph = loader.entry_point(json.get());
}

void RenderDAG::render_entry_point() const {
	for (const std::unique_ptr<IRenderNode>& node : renderGraph) {
		if (node) {
			node->stack_command();
		}
	}
}
