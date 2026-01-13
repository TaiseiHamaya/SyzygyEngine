#ifdef DEBUG_FEATURES_ENABLE

#include "EditorSceneSerializer.h"

using namespace szg;

#include "../RemoteObject/FolderObject.h"
#include "../RemoteObject/RemoteErrorObject.h"
#include "../RemoteObject/RemoteSceneObject.h"
#include "../RemoteObject/RemoteWorldObject.h"
#include "../RemoteObject/WorldInstance/Camera/RemoteCamera3dInstance.h"
#include "../RemoteObject/WorldInstance/Collider/RemoteAABBColliderInstance.h"
#include "../RemoteObject/WorldInstance/Collider/RemoteSphereColliderInstance.h"
#include "../RemoteObject/WorldInstance/Light/RemoteDirectionalLightInstance.h"
#include "../RemoteObject/WorldInstance/Light/RemotePointLightInstance.h"
#include "../RemoteObject/WorldInstance/Mesh/RemoteSkinningMeshInstance.h"
#include "../RemoteObject/WorldInstance/Mesh/RemoteStaticMeshInstance.h"
#include "../RemoteObject/WorldInstance/Primitive/RemoteRect3dInstance.h"
#include "../RemoteObject/WorldInstance/RemoteWorldInstance.h"
#include "../RemoteObject/WorldInstance/StringRect/RemoteStringRectInstance.h"

#include "Engine/Application/Logger.h"
#include "Engine/Assets/Animation/Skeleton/SkeletonLibrary.h"
#include "Engine/Assets/Json/JsonAsset.h"
#include "Engine/Module/World/Camera/ProjectionAdapter/CameraProjectionTypeEnum.h"

#define COLOR_RGB_SERIALIZER
#define COLOR_RGBA_SERIALIZER
#define TRANSFORM2D_SERIALIZER
#define TRANSFORM3D_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

std::unique_ptr<RemoteSceneObject> EditorSceneSerializer::CreateRemoteScene(const std::string& sceneName) {
	std::unique_ptr<RemoteSceneObject> scene = std::make_unique<RemoteSceneObject>();
	if (sceneName.empty() || !std::filesystem::exists(std::format("./Game/Core/Scene/{}", sceneName))) {
		szgWarning("Scene is not found. Name-\'{}\'", sceneName);
		return scene;
	}
	scene->hierarchyName.set_weak(sceneName);

	std::filesystem::path worldsPath = std::format("./Game/Core/Scene/{}/Worlds", sceneName);
	if (!std::filesystem::exists(worldsPath) || !std::filesystem::is_directory(worldsPath)) {
		szgWarning("Worlds folder is not found. Path-\'{}\'", worldsPath.string());
		return scene;
	}

	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(worldsPath)) {
		JsonAsset json{ entry };
		scene->add_child(EditorSceneSerializer::CreateRemoteWorld(json.cget()));
	}

	return scene;
}

std::unique_ptr<RemoteWorldObject> EditorSceneSerializer::CreateRemoteWorld(const nlohmann::json& json) {
	std::unique_ptr<RemoteWorldObject> world = std::make_unique<RemoteWorldObject>();
	json.get_to(world->hierarchyName);
	if (json.contains("Instances") && json["Instances"].is_array()) {
		for (const nlohmann::json& instance : json["Instances"]) {
			world->add_child(CreateRemoteObject(instance));
		}
	}

	json.get_to(world->numLayer);
	return world;
}

std::unique_ptr<IRemoteObject> EditorSceneSerializer::CreateRemoteObject(const nlohmann::json& json) {
	InstanceType type = json.value("Type", InstanceType::ErrorInstance);
	switch (type) {
	case InstanceType::WorldInstance:
		return CreateRemoteInstance(json);
		break;

	case InstanceType::StaticMeshInstance:
		return CreateRemoteStaticMeshInstance(json);
		break;

	case InstanceType::SkinningMeshInstance:
		return CreateRemoteSkinningMeshInstance(json);
		break;

	case InstanceType::Rect3dInstance:
		return CreateRemoteRedct3dInstance(json);
		break;

	case InstanceType::StringRectInstance:
		return CreateRemoteStringRectInstance(json);
		break;

	case InstanceType::CameraInstance:
		return CreateRemoteCamera3DInstance(json);
		break;

	case InstanceType::AABBColliderInstance:
		return CreateRemoteAABBColliderInstance(json);
		break;

	case InstanceType::SphereColliderInstance:
		return CreateRemoteSphereColliderInstance(json);
		break;

	case InstanceType::DirectionalLightInstance:
		return CreateRemoteDirectionalLightInstance(json);
		break;

	case InstanceType::PointLightInstance:
		return CreateRemotePointLightInstance(json);
		break;

	case InstanceType::DebugFolder:
		return CreateRemoteFolder(json);
		break;
	case InstanceType::DebugScene:
	case InstanceType::DebugWorldRoot:
	case InstanceType::ErrorInstance:
	default:
		return std::make_unique<RemoteErrorObject>(std::format("Undefined remote object type.({})", static_cast<u32>(type)));
		break;
	}
}

std::unique_ptr<IRemoteObject> EditorSceneSerializer::CreateRemoteFolder(const nlohmann::json& json) {
	std::unique_ptr<FolderObject> folder = std::make_unique<FolderObject>();
	json.get_to(folder->hierarchyName);
	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instance : json["Children"]) {
			folder->add_child(CreateRemoteObject(instance));
		}
	}
	return folder;
}

std::unique_ptr<IRemoteObject> EditorSceneSerializer::CreateRemoteInstance(const nlohmann::json& json) {
	std::unique_ptr<RemoteWorldInstance> result = std::make_unique<RemoteWorldInstance>();
	json.get_to(result->hierarchyName);
	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instance : json["Children"]) {
			result->add_child(CreateRemoteObject(instance));
		}
	}
	json.get_to(result->transform);
	json.get_to(result->isUseRuntime);

	return result;
}

std::unique_ptr<IRemoteObject> EditorSceneSerializer::CreateRemoteStaticMeshInstance(const nlohmann::json& json) {
	std::unique_ptr<RemoteStaticMeshInstance> result = std::make_unique<RemoteStaticMeshInstance>();
	json.get_to(result->hierarchyName);
	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instance : json["Children"]) {
			result->add_child(CreateRemoteObject(instance));
		}
	}
	json.get_to(result->transform);
	json.get_to(result->isUseRuntime);
	json.get_to(result->isDraw);
	if (json.contains("MeshName")) {
		json["MeshName"].get_to(result->meshName);
	}
	json.get_to(result->layer);
	if (json.contains("Materials") && json["Materials"].is_array()) {
		for (const nlohmann::json& jMaterial : json["Materials"]) {
			RemoteStaticMeshInstance::Material& material = result->materials.emplace_back();
			if (jMaterial.contains("Texture")) {
				material.texture = jMaterial.at("Texture").get<std::string>();
			}
			if (jMaterial.contains("Color")) {
				jMaterial.at("Color").get_to(material.color);
			}
			if (jMaterial.contains("UV Transform")) {
				jMaterial.at("UV Transform").get_to(material.uvTransform);
			}
			if (jMaterial.contains("LightingType")) {
				material.lightingType = static_cast<LighingType>(jMaterial.at("LightingType").get<std::underlying_type_t<LighingType>>());
			}
			if (jMaterial.contains("Shininess")) {
				jMaterial.at("Shininess").get_to(material.shininess);
			}
		}
	}

	return result;
}

std::unique_ptr<IRemoteObject> EditorSceneSerializer::CreateRemoteSkinningMeshInstance(const nlohmann::json& json) {
	std::unique_ptr<RemoteSkinningMeshInstance> result = std::make_unique<RemoteSkinningMeshInstance>();
	json.get_to(result->hierarchyName);
	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instance : json["Children"]) {
			result->add_child(CreateRemoteObject(instance));
		}
	}
	json.get_to(result->transform);
	json.get_to(result->isUseRuntime);
	json.get_to(result->isDraw);
	if (json.contains("MeshName")) {
		json["MeshName"].get_to(result->meshName);
	}
	result->skeleton = SkeletonLibrary::GetSkeleton(result->meshName);
	json.get_to(result->layer);
	if (json.contains("Materials") && json["Materials"].is_array()) {
		for (const nlohmann::json& jMaterial : json["Materials"]) {
			RemoteSkinningMeshInstance::Material& material = result->materials.emplace_back();
			if (jMaterial.contains("Texture")) {
				material.texture = jMaterial.at("Texture").get<std::string>();
			}
			if (jMaterial.contains("Color")) {
				jMaterial.at("Color").get_to(material.color);
			}
			if (jMaterial.contains("UV Transform")) {
				jMaterial.at("UV Transform").get_to(material.uvTransform);
			}
			if (jMaterial.contains("LightingType")) {
				material.lightingType = static_cast<LighingType>(jMaterial.at("LightingType").get<std::underlying_type_t<LighingType>>());
			}
			if (jMaterial.contains("Shininess")) {
				jMaterial.at("Shininess").get_to(material.shininess);
			}
		}
	}
	if (json.contains("AnimationName")) {
		json["AnimationName"].get_to(result->animationName);
	}
	json.get_to(result->isLoop);

	return result;
}

std::unique_ptr<IRemoteObject> EditorSceneSerializer::CreateRemoteRedct3dInstance(const nlohmann::json& json) {
	std::unique_ptr<RemoteRect3dInstance> result = std::make_unique<RemoteRect3dInstance>();
	json.get_to(result->hierarchyName);


	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instance : json["Children"]) {
			result->add_child(CreateRemoteObject(instance));
		}
	}

	json.get_to(result->transform);
	json.get_to(result->isUseRuntime);

	json.get_to(result->isDraw);
	json.get_to(result->layer);

	json.get_to(result->size);
	json.get_to(result->pivot);
	json.get_to(result->isFlipY);

	nlohmann::json jMaterial = json.value("Material", nlohmann::json::object());
	result->material.texture = jMaterial.value("Texture", "Error.png");
	jMaterial.get_to(result->material.color);
	jMaterial.get_to(result->material.uvTransform);
	result->material.lightingType = jMaterial.value("LightingType", LighingType::None);
	jMaterial.get_to(result->material.shininess);

	return result;
}

std::unique_ptr<IRemoteObject> EditorSceneSerializer::CreateRemoteStringRectInstance(const nlohmann::json& json) {
	std::unique_ptr<RemoteStringRectInstance> result = std::make_unique<RemoteStringRectInstance>();
	json.get_to(result->hierarchyName);

	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instance : json["Children"]) {
			result->add_child(CreateRemoteObject(instance));
		}
	}

	json.get_to(result->transform);
	json.get_to(result->isUseRuntime);

	json.get_to(result->isDraw);
	json.get_to(result->layer);

	result->font = json.value("Font", "");
	json.get_to(result->pivot);
	result->text.set_weak(json.value("Text", "Sample Text"));
	json.get_to(result->fontSize);
	json.get_to(result->color);

	return result;
}

std::unique_ptr<IRemoteObject> EditorSceneSerializer::CreateRemoteCamera3DInstance(const nlohmann::json& json) {
	std::unique_ptr<RemoteCamera3dInstance> result = std::make_unique<RemoteCamera3dInstance>();
	json.get_to(result->hierarchyName);
	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instance : json["Children"]) {
			result->add_child(CreateRemoteObject(instance));
		}
	}
	json.get_to(result->transform);
	json.get_to(result->isUseRuntime);

	nlohmann::json projectionJson = json.value("Projection", nlohmann::json::object());
	CameraProjectionTypeEnum projectionType = projectionJson.value("Type", szg::CameraProjectionTypeEnum::Undefined);
	switch (projectionType) {
	case szg::CameraProjectionTypeEnum::Perspective:
	{
		RemoteCamera3dInstance::PerspectiveParameters perspectiveParams;
		projectionJson.get_to(perspectiveParams.fovY);
		projectionJson.get_to(perspectiveParams.aspectRatio);
		projectionJson.get_to(perspectiveParams.nearClip);
		projectionJson.get_to(perspectiveParams.farClip);
		result->projectionParameters = perspectiveParams;
	}
	break;
	case szg::CameraProjectionTypeEnum::Orthographic:
	{
		RemoteCamera3dInstance::OrthroParameters orthroParams;
		projectionJson.get_to(orthroParams.left);
		projectionJson.get_to(orthroParams.right);
		projectionJson.get_to(orthroParams.bottom);
		projectionJson.get_to(orthroParams.top);
		projectionJson.get_to(orthroParams.nearClip);
		projectionJson.get_to(orthroParams.farClip);
		result->projectionParameters = orthroParams;
	}
	break;
	default:
		break;
	}

	return result;
}

std::unique_ptr<IRemoteObject> EditorSceneSerializer::CreateRemoteSphereColliderInstance(const nlohmann::json& json) {
	std::unique_ptr<RemoteSphereColliderInstance> result = std::make_unique<RemoteSphereColliderInstance>();
	json.get_to(result->hierarchyName);
	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instance : json["Children"]) {
			result->add_child(CreateRemoteObject(instance));
		}
	}
	json.get_to(result->transform);
	json.get_to(result->isUseRuntime);

	json.get_to(result->radius);

	return result;
}

std::unique_ptr<IRemoteObject> EditorSceneSerializer::CreateRemoteAABBColliderInstance(const nlohmann::json& json) {
	std::unique_ptr<RemoteAABBColliderInstance> result = std::make_unique<RemoteAABBColliderInstance>();
	json.get_to(result->hierarchyName);
	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instance : json["Children"]) {
			result->add_child(CreateRemoteObject(instance));
		}
	}
	json.get_to(result->transform);
	json.get_to(result->isUseRuntime);

	json.get_to(result->size);
	json.get_to(result->offset);

	return result;
}

std::unique_ptr<IRemoteObject> EditorSceneSerializer::CreateRemoteDirectionalLightInstance(const nlohmann::json& json) {
	std::unique_ptr<RemoteDirectionalLightInstance> result = std::make_unique<RemoteDirectionalLightInstance>();
	json.get_to(result->hierarchyName);

	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instance : json["Children"]) {
			result->add_child(CreateRemoteObject(instance));
		}
	}

	json.get_to(result->transform);
	json.get_to(result->isUseRuntime);

	json.get_to(result->color);
	json.get_to(result->intensity);
	json.get_to(result->direction);
	json.get_to(result->influenceLayer);
	return result;
}

std::unique_ptr<IRemoteObject> EditorSceneSerializer::CreateRemotePointLightInstance(const nlohmann::json& json) {
	std::unique_ptr<RemotePointLightInstance> result = std::make_unique<RemotePointLightInstance>();
	json.get_to(result->hierarchyName);
	if (json.contains("Children") && json["Children"].is_array()) {
		for (const nlohmann::json& instance : json["Children"]) {
			result->add_child(CreateRemoteObject(instance));
		}
	}
	json.get_to(result->transform);
	json.get_to(result->isUseRuntime);

	json.get_to(result->color);
	json.get_to(result->intensity);
	json.get_to(result->radius);
	json.get_to(result->decay);
	json.get_to(result->influenceLayer);
	return result;
}

#endif // DEBUG_FEATURES_ENABLE
