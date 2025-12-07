#pragma once

#include <json.hpp>

#include <Library/Utility/Template/Reference.h>

namespace szg {

class WorldRoot;
class WorldInstance;

class WorldInstanceLoader {
public:
	void setup(Reference<WorldRoot> worldRoot_);

	void entry_point(const nlohmann::json& json, Reference<WorldInstance> parent);

private:
	void create_world_instance(const nlohmann::json& json, Reference<WorldInstance> parent);

	void create_static_mesh_instance(const nlohmann::json& json, Reference<WorldInstance> parent);
	void create_skinning_mesh_instance(const nlohmann::json& json, Reference<WorldInstance> parent);
	void create_rect3d_instance(const nlohmann::json& json, Reference<WorldInstance> parent);
	void create_string_rect_instance(const nlohmann::json& json, Reference<WorldInstance> parent);
	
	void create_camera3d_instance(const nlohmann::json& json, Reference<WorldInstance> parent);
	
	void create_sphere_collider_instance(const nlohmann::json& json, Reference<WorldInstance> parent);
	void create_aabb_collider_instance(const nlohmann::json& json, Reference<WorldInstance> parent);
	
	void create_directional_light_instance(const nlohmann::json& json, Reference<WorldInstance> parent);
	void create_point_light_instance(const nlohmann::json& json, Reference<WorldInstance> parent);

	void calculate_folder_hierarchy(const nlohmann::json& json, Reference<WorldInstance> parent);

private:
	Reference<WorldRoot> worldRoot;
};

}; // szg
