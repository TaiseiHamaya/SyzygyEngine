#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <filesystem>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Runtime/Scene/SceneAssetCollection.h"

namespace szg {

class RemoteSceneObject;
class EditorRenderDAG;

class EditorAssetSaver final {
public:
	EditorAssetSaver() = default;
	~EditorAssetSaver() = default;

	SZG_CLASS_MOVE_ONLY(EditorAssetSaver)

public:
	void setup(Reference<const EditorRenderDAG> dagEditor_);

	/// <summary>
	/// "filePath / Assets.json"に出力
	/// </summary>
	/// <param name="filePath"></param>
	void save(const std::filesystem::path& filePath);

private:
	void collect_assets();

	void collect_shaders();

	void save_to_json(const std::filesystem::path& filePath);

private:
	Reference<const EditorRenderDAG> dagEditor;

	SceneAssetCollection sceneAssetCollection;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
