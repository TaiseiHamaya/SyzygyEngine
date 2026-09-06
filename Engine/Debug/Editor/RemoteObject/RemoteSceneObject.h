#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "IRemoteObject.h"

#include <vector>

namespace szg {

class Scene;
class RemoteWorldObject;
class EditorRenderDAG;

class RemoteSceneObject final : public IRemoteObject {
public:
	friend class EditorSceneSerializer;

public:
	RemoteSceneObject();
	~RemoteSceneObject();

	SZG_CLASS_DEFAULT(RemoteSceneObject)

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

	constexpr InstanceType instance_type() const { return InstanceType::DebugScene; }

public:
	size_t world_size() const;

	std::string name() const;

	const std::vector<std::unique_ptr<RemoteWorldObject>>& remote_worlds_imm() const;

	void set_editor(Reference<EditorRenderDAG> renderDAGEditor_);

private:
	Reference<Scene> self;

	std::vector<std::unique_ptr<RemoteWorldObject>> remoteWorlds;

	Reference<EditorRenderDAG> renderDAGEditor;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
