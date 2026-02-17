#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <json.hpp>

#include <filesystem>
#include <unordered_map>

#include "../Window/RenderDAG/EditorRenderDAG.h"
#include "../Window/RenderDAG/PostEffectImNode.h"
#include "../Window/RenderDAG/ResultImNode.h"
#include "../Window/RenderDAG/StaticTextureImNode.h"
#include "../Window/RenderDAG/WorldLayerRenderImNode.h"

/// <summary>
/// RenderPath保存用関数オブジェクト
/// </summary>
namespace szg {

class RenderDAGImNodeSaver {
public:
	void entry_point(const std::filesystem::path& filePath, const std::unordered_map<u64, EditorRenderDAG::DAGNodeType>& nodes);

	nlohmann::json operator()(const std::shared_ptr<WorldLayerRenderImNode>& node);

	nlohmann::json operator()(const std::shared_ptr<PostEffectImNode>& node);

	nlohmann::json operator()(const std::shared_ptr<StaticTextureImNode>& node);

	nlohmann::json operator()(const std::shared_ptr<ResultImNode>&);
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
