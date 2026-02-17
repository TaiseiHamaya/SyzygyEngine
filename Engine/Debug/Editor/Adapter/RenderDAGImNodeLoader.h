#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <memory>

#include <ImNodeFlow/include/ImNodeFlow.h>
#include <json.hpp>

#include <Library/Utility/Template/Reference.h>

#include "Engine/Debug/Editor/Window/RenderDAG/EditorRenderDAG.h"

namespace szg {

class WorldLayerRenderImNode;
class PostEffectImNode;
class StaticTextureImNode;

/// <summary>
/// RenderDAGmNodeのLoader
/// </summary>
class RenderDAGImNodeLoader {
public:
	u64 entry_point(const std::string& sceneName, Reference<ImFlow::ImNodeFlow> imNodeFlow_, std::unordered_map<u64, EditorRenderDAG::DAGNodeType>& nodes);

private:
	std::shared_ptr<WorldLayerRenderImNode> load_as_world_render(const nlohmann::json& json);

	std::shared_ptr<PostEffectImNode> load_as_post_effect(const nlohmann::json& json);

	std::shared_ptr<StaticTextureImNode> load_as_static_texture(const nlohmann::json& json);

private:
	u64 counter{ 1 };
	Reference<ImFlow::ImNodeFlow> imNodeFlow;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
