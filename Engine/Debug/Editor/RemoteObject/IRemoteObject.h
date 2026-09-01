#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <memory>
#include <string>

#include <json.hpp>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

#include "../Core/EditorSelectObject.h"
#include "Engine/Debug/Editor/Adapter/Field/EditorValueField.h"
#include "Engine/Module/World/WorldInstanceType.h"

class Affine;

namespace szg {

class EditorSceneView;
class RemoteWorldObject;
class EditorDebugCamera;

class IRemoteObject {
	friend class EditorSceneSerializer;

public:
	IRemoteObject() = default;
	virtual ~IRemoteObject() = default;

	SZG_CLASS_DEFAULT(IRemoteObject)

public:
	virtual void setup() = 0;

	virtual void update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) = 0;

	virtual void draw_inspector() = 0;

	virtual void draw_hierarchy(Reference<const EditorSelectObject> select) = 0;

	virtual std::unique_ptr<IRemoteObject> move_force(Reference<const IRemoteObject> child) = 0;

	virtual void reparent(Reference<IRemoteObject> remoteObject);

	virtual void add_child(std::unique_ptr<IRemoteObject> child) = 0;

	virtual nlohmann::json serialize() const = 0;

	virtual Reference<const RemoteWorldObject> query_world() const;

	virtual void on_spawn() = 0;

	virtual void on_destroy() = 0;

	virtual constexpr InstanceType instance_type() const = 0;

public:
	Reference<IRemoteObject> get_parent() const;

protected:
	bool isOpen{ false };
	EditorValueField<std::string> hierarchyName{ "Name" };

	Reference<IRemoteObject> parent;

public:
	static void Setup(Reference<EditorSceneView> sceneView);

protected:
	static inline Reference<EditorSceneView> sceneView{ nullptr };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
