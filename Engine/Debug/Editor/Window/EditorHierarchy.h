#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "IEditorWindow.h"

#include <memory>
#include <string>

#include <Library/Utility/Template/Reference.h>

#include "../RemoteObject/RemoteSceneObject.h"

namespace szg {

class IRemoteObject;
class EditorSelectObject;
class EditorSceneView;

class EditorHierarchy : public IEditorWindow {
public:
	void setup(Reference<EditorSelectObject> select_, Reference<EditorSceneView> sceneView);

	void finalize();

	void update_preview();

	void load(const std::string& sceneName);
	void save(const std::filesystem::path& path) const;

	void draw() override;

public:
	std::string current_scene_name() const;
	const std::vector<std::unique_ptr<RemoteWorldObject>>& world_list() const;
	Reference<const RemoteSceneObject> scene_imm() const;

private:
	std::unique_ptr<RemoteSceneObject> scene;

	std::string searchString;
	std::string menuString;

	Reference<EditorSelectObject> select{ nullptr };
	Reference<EditorSceneView> sceneView{ nullptr };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
