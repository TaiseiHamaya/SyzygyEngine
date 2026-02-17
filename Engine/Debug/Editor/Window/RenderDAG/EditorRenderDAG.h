#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IEditorWindow.h"

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <variant>

#include <ImNodeFlow/include/ImNodeFlow.h>

namespace szg {

class WorldLayerRenderImNode;
class PostEffectImNode;
class StaticTextureImNode;
class ResultImNode;
class EditorHierarchy;

class EditorRenderDAG final : public IEditorWindow {
public:
	EditorRenderDAG();
	~EditorRenderDAG() override = default;

	SZG_CLASS_MOVE_ONLY(EditorRenderDAG)

public:
	using DAGNodeType =
		std::variant<
		std::shared_ptr<WorldLayerRenderImNode>,
		std::shared_ptr<PostEffectImNode>,
		std::shared_ptr<StaticTextureImNode>,
		std::shared_ptr<ResultImNode>
		>;

public:
	void initialize();

	void load(const std::string& sceneName);

	void finalize();

	void draw() override;

	/// <summary>
	/// "filePath / RenderPath.json"に出力
	/// </summary>
	/// <param name="filePath"></param>
	void save(const std::filesystem::path& filePath);

public:
	const std::unordered_map<u64, DAGNodeType>& nodes_imm() const;

private:
	void generate_result_node();

private:
	u64 nodeCounter{ 0 };

	std::unique_ptr<ImFlow::ImNodeFlow> imNodeFlow{};
	std::unordered_map<u64, DAGNodeType> nodes;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
