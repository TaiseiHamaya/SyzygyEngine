#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <set>
#include <string>

namespace szg {

class EditorSceneList {
public:
	void initialize();
	void finalize();

public:
	bool scene_list_gui(std::string& current);

	void add_scene(const std::string& sceneName);

private:
	void export_scene_list();

private:
	std::set<std::string> sceneList;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
