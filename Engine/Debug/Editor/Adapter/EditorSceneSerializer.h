#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <memory>
#include <string>

#include <json.hpp>

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "../RemoteObject/RemoteSceneObject.h"
#include "../RemoteObject/RemoteWorldObject.h"

namespace szg {

class EditorSceneSerializer final {
public:
	EditorSceneSerializer() = default;
	~EditorSceneSerializer() = default;

	SZG_CLASS_MOVE_ONLY(EditorSceneSerializer)

public:
	static std::unique_ptr<RemoteSceneObject> CreateRemoteScene(const std::string& sceneName);
	static std::unique_ptr<RemoteWorldObject> CreateRemoteWorld(const nlohmann::json& json);
	static std::unique_ptr<IRemoteObject> CreateRemoteObject(const nlohmann::json& json);

private:
	static std::unique_ptr<IRemoteObject> CreateRemoteFolder(const nlohmann::json& json);

	static std::unique_ptr<IRemoteObject> CreateRemoteInstance(const nlohmann::json& json);

	static std::unique_ptr<IRemoteObject> CreateRemoteStaticMeshInstance(const nlohmann::json& json);
	static std::unique_ptr<IRemoteObject> CreateRemoteSkinningMeshInstance(const nlohmann::json& json);
	static std::unique_ptr<IRemoteObject> CreateRemoteRedct3dInstance(const nlohmann::json& json);
	static std::unique_ptr<IRemoteObject> CreateRemoteStringRectInstance(const nlohmann::json& json);

	static std::unique_ptr<IRemoteObject> CreateRemoteCamera3DInstance(const nlohmann::json& json);

	static std::unique_ptr<IRemoteObject> CreateRemoteSphereColliderInstance(const nlohmann::json& json);
	static std::unique_ptr<IRemoteObject> CreateRemoteAABBColliderInstance(const nlohmann::json& json);

	static std::unique_ptr<IRemoteObject> CreateRemoteDirectionalLightInstance(const nlohmann::json& json);
	static std::unique_ptr<IRemoteObject> CreateRemotePointLightInstance(const nlohmann::json& json);

	static std::unique_ptr<IRemoteObject> CreateRemoteEmitterInstance(const nlohmann::json& json);
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
