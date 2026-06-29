#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IRemoteObject.h"

#include <memory>
#include <string>
#include <vector>

#include "Engine/Debug/Editor/RemoteObject/WorldObject/RemoteColliderGroupRegistry.h"

namespace szg {

class WorldCluster;
class FolderObject;
class RemoteWorldInstance;

class RemoteWorldObject final : public IRemoteObject {
public:
	friend class EditorSceneSerializer;

public:
	RemoteWorldObject();
	~RemoteWorldObject();

	SZG_CLASS_MOVE_ONLY(RemoteWorldObject)

public:
	void setup() override;

	void update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) override;

	void draw_inspector() override;

	void draw_hierarchy(Reference<const EditorSelectObject> select) override;

	std::unique_ptr<IRemoteObject> move_force(Reference<const IRemoteObject> child) override;

	void reparent(Reference<IRemoteObject> remoteObject) override;

	void add_child(std::unique_ptr<IRemoteObject> child) override;

	nlohmann::json serialize() const override;

	Reference<const RemoteWorldObject> query_world() const override;

	void on_spawn() override;

	void on_destroy() override;

	constexpr InstanceType instance_type() const { return InstanceType::DebugWorldRoot; }

public:
	const std::string& world_name() const;

	u32 get_id() const;

	Reference<const RemoteColliderGroupRegistry> collider_group_registry_imm() const;

private:
	Reference<WorldCluster> self;

	std::vector<std::unique_ptr<IRemoteObject>> children;

	u32 id;

	EditorValueField<u32> numLayer{ "NumLayer", 1 };

	RemoteColliderGroupRegistry colliderGroupRegistry;

private:
	inline static u32 nextUseId{ 0 };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
