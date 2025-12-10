#include "WorldInstanceLoader.h"

using namespace szg;

#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/Module/Manager/World/WorldRoot.h"
#include "Engine/Module/World/Camera/Camera3D.h"
#include "Engine/Module/World/Collision/Collider/AABBCollider.h"
#include "Engine/Module/World/Collision/Collider/SphereCollider.h"
#include "Engine/Module/World/Light/DirectionalLight/DirectionalLightInstance.h"
#include "Engine/Module/World/Light/PointLight/PointLightInstance.h"
#include "Engine/Module/World/Mesh/Primitive/Rect3d.h"
#include "Engine/Module/World/Mesh/Primitive/StringRectInstance.h"
#include "Engine/Module/World/Mesh/SkinningMeshInstance.h"
#include "Engine/Module/World/Mesh/StaticMeshInstance.h"
#include "Engine/Module/World/WorldInstance/WorldInstance.h"
#include "Engine/Module/World/WorldInstanceType.h"
#include "Engine/Module/Manager/RuntimeStorage/RuntimeStorage.h"

#define TRANSFORM3D_SERIALIZER
#define TRANSFORM2D_SERIALIZER
#define COLOR3_SERIALIZER
#define COLOR4_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

void WorldInstanceLoader::setup(Reference<WorldRoot> worldRoot_) {
	worldRoot = worldRoot_;
}

void WorldInstanceLoader::entry_point(const nlohmann::json& json, Reference<WorldInstance> parent) {
	if (!json.contains("Type")) {
		return;
	}
	InstanceType type = json.value("Type", InstanceType::ErrorInstance);
	switch (type) {
	case InstanceType::WorldInstance:
		create_world_instance(json, parent);
		break;

	case InstanceType::StaticMeshInstance:
		create_static_mesh_instance(json, parent);
		break;

	case InstanceType::SkinningMeshInstance:
		create_skinning_mesh_instance(json, parent);
		break;

	case InstanceType::Rect3dInstance:
		create_rect3d_instance(json, parent);
		break;

	case InstanceType::StringRectInstance:
		create_string_rect_instance(json, parent);
		break;

	case InstanceType::CameraInstance:
		create_camera3d_instance(json, parent);
		break;

	case InstanceType::AABBColliderInstance:
		create_aabb_collider_instance(json, parent);
		break;

	case InstanceType::SphereColliderInstance:
		create_sphere_collider_instance(json, parent);
		break;

	case InstanceType::DirectionalLightInstance:
		create_directional_light_instance(json, parent);
		break;

	case InstanceType::PointLightInstance:
		create_point_light_instance(json, parent);
		break;

	case InstanceType::DebugFolder:
		calculate_folder_hierarchy(json, parent);
		break;

	case InstanceType::ErrorInstance:
	default:
		szgWarning("WorldInstanceLoader::entry_point: Unknown Instance Type");
		break;
	}
}

void WorldInstanceLoader::create_world_instance(const nlohmann::json& json, Reference<WorldInstance> parent) {
	auto instance = worldRoot->instantiate<WorldInstance>(parent);

	instance->get_transform().copy(json["Local transform"].get<Transform3D>());
	if (json.value("Use runtime", false) && !json.value("Name", "").empty()) {
		RuntimeStorage::GetValueList("RuntimeInstance").emplace(json["Name"], instance);
	}

	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instanceJson : json["Children"]) {
			entry_point(instanceJson, instance);
		}
	}
}

void WorldInstanceLoader::create_static_mesh_instance(const nlohmann::json& json, Reference<WorldInstance> parent) {
	auto instance = worldRoot->instantiate<StaticMeshInstance>(parent);

	instance->get_transform().copy(json["Local transform"].get<Transform3D>());
	instance->reset_mesh(json.value("MeshName", ""));
	instance->set_draw(json.value("IsDraw", true));
	instance->set_layer(json.value("Layer", 0u));

	if (json.contains("Materials") && json["Materials"].is_array()) {
		const auto& materialsJson = json["Materials"];
		for (u32 i = 0; i < materialsJson.size() && i < instance->get_materials().size(); ++i) {
			auto& material = instance->get_materials()[i];
			const auto& materialJson = materialsJson[i];
			if (materialJson.contains("Texture")) {
				material.texture = TextureLibrary::GetTexture(materialJson["Texture"].get<std::string>());
			}
			if (materialJson.contains("Color")) {
				materialJson["Color"].get_to(material.color);
			}
			if (materialJson.contains("UV Transform")) {
				materialJson["UV Transform"].get_to(material.uvTransform);
			}
			if (materialJson.contains("LightingType")) {
				material.lightingType = static_cast<LighingType>(materialJson["LightingType"].get<std::underlying_type_t<LighingType>>());
			}
			if (materialJson.contains("Shininess")) {
				materialJson["Shininess"].get_to(material.shininess);
			}
		}
	}

	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instanceJson : json["Children"]) {
			entry_point(instanceJson, instance);
		}
	}
}

void WorldInstanceLoader::create_skinning_mesh_instance(const nlohmann::json& json, Reference<WorldInstance> parent) {
	auto instance = worldRoot->instantiate<SkinningMeshInstance>(parent);

	instance->get_transform().copy(json["Local transform"].get<Transform3D>());
	if (json.value("Use runtime", false) && !json.value("Name", "").empty()) {
		RuntimeStorage::GetValueList("RuntimeInstance").emplace(json["Name"], instance);
	}

	instance->reset_animated_mesh(json.value("MeshName", ""), json.value("AnimationName", ""), json.value("IsLoop", false));
	instance->set_draw(json.value("IsDraw", true));
	instance->set_layer(json.value("Layer", 0u));

	if (json.contains("Materials") && json["Materials"].is_array()) {
		const auto& materialsJson = json["Materials"];
		for (u32 i = 0; i < materialsJson.size() && i < instance->get_materials().size(); ++i) {
			auto& material = instance->get_materials()[i];
			const auto& materialJson = materialsJson[i];
			if (materialJson.contains("Texture")) {
				material.texture = TextureLibrary::GetTexture(materialJson["Texture"].get<std::string>());
			}
			if (materialJson.contains("Color")) {
				materialJson["Color"].get_to(material.color);
			}
			if (materialJson.contains("UV Transform")) {
				materialJson["UV Transform"].get_to(material.uvTransform);
			}
			if (materialJson.contains("LightingType")) {
				material.lightingType = static_cast<LighingType>(materialJson["LightingType"].get<std::underlying_type_t<LighingType>>());
			}
			if (materialJson.contains("Shininess")) {
				materialJson["Shininess"].get_to(material.shininess);
			}
		}
	}

	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instanceJson : json["Children"]) {
			entry_point(instanceJson, instance);
		}
	}
}

void WorldInstanceLoader::create_rect3d_instance(const nlohmann::json& json, Reference<WorldInstance> parent) {
	auto instance = worldRoot->instantiate<Rect3d>(parent);

	instance->get_transform().copy(json["Local transform"].get<Transform3D>());
	if (json.value("Use runtime", false) && !json.value("Name", "").empty()) {
		RuntimeStorage::GetValueList("RuntimeInstance").emplace(json["Name"], instance);
	}

	instance->initialize(
		json.value("Size", CVector2::BASIS), json.value("Pivot", CVector2::ZERO)
	);
	instance->set_flip_y(json.value("IsFlipY", false));

	instance->set_draw(json.value("IsDraw", true));
	instance->set_layer(json.value("Layer", 0u));

	nlohmann::json materialJson = json.value("Material", nlohmann::json::object());
	instance->get_material().texture = TextureLibrary::GetTexture(materialJson.value("Texture", ""));
	instance->get_material().color = materialJson.value("Color", CColor4::WHITE);
	instance->get_material().uvTransform.copy(materialJson.value("UV Transform", Transform2D{}));
	instance->get_material().lightingType = materialJson.value("LightingType", LighingType::None);
	instance->get_material().shininess = materialJson.value("Shininess", 50.0f);

	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instanceJson : json["Children"]) {
			entry_point(instanceJson, instance);
		}
	}
}

void WorldInstanceLoader::create_string_rect_instance(const nlohmann::json& json, Reference<WorldInstance> parent) {
	auto instance = worldRoot->instantiate<StringRectInstance>(parent);
	instance->get_transform().copy(json["Local transform"].get<Transform3D>());
	if (json.value("Use runtime", false) && !json.value("Name", "").empty()) {
		RuntimeStorage::GetValueList("RuntimeInstance").emplace(json["Name"], instance);
	}

	instance->initialize(
		json.value("Font", ""),
		json.value("Size", 10.0f),
		json.value("Pivot", CVector2::ZERO)
	);

	instance->reset_string(json.value("Text", ""));
	instance->set_layer(json.value("Layer", 0u));
	instance->set_draw(json.value("IsDraw", true));
	instance->get_material().color = json.value("Color", CColor4::WHITE);

	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instanceJson : json["Children"]) {
			entry_point(instanceJson, instance);
		}
	}
}

void WorldInstanceLoader::create_camera3d_instance(const nlohmann::json& json, Reference<WorldInstance> parent) {
	auto instance = worldRoot->instantiate<Camera3D>(parent);

	instance->get_transform().copy(json["Local transform"].get<Transform3D>());
	if (json.value("Use runtime", false) && !json.value("Name", "").empty()) {
		RuntimeStorage::GetValueList("RuntimeInstance").emplace(json["Name"], instance);
	}

	instance->set_perspective_fov_info(
		json.value("FovY", 0.45f),
		json.value("AspectRatio", 1.7777f),
		json.value("NearClip", 0.1f),
		json.value("FarClip", 1000.0f)
	);

	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instanceJson : json["Children"]) {
			entry_point(instanceJson, instance);
		}
	}
}

void WorldInstanceLoader::create_sphere_collider_instance(const nlohmann::json& json, Reference<WorldInstance> parent) {
	auto instance = worldRoot->instantiate<SphereCollider>(parent);

	instance->get_transform().copy(json["Local transform"].get<Transform3D>());
	if (json.value("Use runtime", false) && !json.value("Name", "").empty()) {
		RuntimeStorage::GetValueList("RuntimeInstance").emplace(json["Name"], instance);
	}

	instance->set_radius(json.value("Radius", 1.0f));

	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instanceJson : json["Children"]) {
			entry_point(instanceJson, instance);
		}
	}
}

void WorldInstanceLoader::create_aabb_collider_instance(const nlohmann::json& json, Reference<WorldInstance> parent) {
	auto instance = worldRoot->instantiate<AABBCollider>(parent, json.value("Size", CVector3::ONE), json.value("Offset", CVector3::ZERO));

	instance->get_transform().copy(json["Local transform"].get<Transform3D>());
	if (json.value("Use runtime", false) && !json.value("Name", "").empty()) {
		RuntimeStorage::GetValueList("RuntimeInstance").emplace(json["Name"], instance);
	}

	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instanceJson : json["Children"]) {
			entry_point(instanceJson, instance);
		}
	}
}

void WorldInstanceLoader::create_directional_light_instance(const nlohmann::json& json, Reference<WorldInstance> parent) {
	auto instance = worldRoot->instantiate<DirectionalLightInstance>(parent);

	instance->get_transform().copy(json["Local transform"].get<Transform3D>());
	if (json.value("Use runtime", false) && !json.value("Name", "").empty()) {
		RuntimeStorage::GetValueList("RuntimeInstance").emplace(json["Name"], instance);
	}

	instance->light_data_mut().color = json.value("Color", CColor3::WHITE);
	instance->light_data_mut().intensity = json.value("Intensity", 1.0f);
	instance->light_data_mut().direction = json.value("Direction", CVector3::DOWN);
	instance->set_influence_layer(json.value("Influence Layer", 0u));

	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instanceJson : json["Children"]) {
			entry_point(instanceJson, instance);
		}
	}
}

void WorldInstanceLoader::create_point_light_instance(const nlohmann::json& json, Reference<WorldInstance> parent) {
	auto instance = worldRoot->instantiate<PointLightInstance>(parent);

	instance->get_transform().copy(json["Local transform"].get<Transform3D>());
	if (json.value("Use runtime", false) && !json.value("Name", "").empty()) {
		RuntimeStorage::GetValueList("RuntimeInstance").emplace(json["Name"], instance);
	}

	instance->light_data_mut().color = json.value("Color", CColor3::WHITE);
	instance->light_data_mut().intensity = json.value("Intensity", 1.0f);
	instance->light_data_mut().radius = json.value("Radius", 1.0f);
	instance->light_data_mut().decay = json.value("Decay", 0.0f);
	instance->set_influence_layer(json.value("Influence Layer", 0u));

	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instanceJson : json["Children"]) {
			entry_point(instanceJson, instance);
		}
	}
}

void WorldInstanceLoader::calculate_folder_hierarchy(const nlohmann::json& json, Reference<WorldInstance> parent) {
	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instanceJson : json["Children"]) {
			entry_point(instanceJson, parent);
		}
	}
}
