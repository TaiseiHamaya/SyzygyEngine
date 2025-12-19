#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "IEditorWindow.h"

#include <unordered_map>

#include "EditorWorldView/EditorWorldView.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/TempTexture.h"
#include "Engine/Module/DrawExecutor/LightingExecutor/DirectionalLightingExecutor.h"
#include "Engine/Module/DrawExecutor/Mesh/StaticMeshDrawManager.h"
#include "Engine/Module/DrawExecutor/Mesh/Primitive/Rect3dDrawManager.h"
#include "Engine/Module/DrawExecutor/Mesh/Primitive/StringRectDrawManager.h"
#include "Engine/Module/Render/RenderPath/RenderPath.h"

#include <Library/Math/Vector2.h>

struct ImDrawList;

namespace szg {

class EditorGizmo;
class EditorHierarchy;

class EditorSceneView final : public IEditorWindow {
private:
	struct LayerAccessor {
		u32 layer;
		EditorWorldView view;
	};

public:
	EditorSceneView() = default;
	~EditorSceneView() = default;

	SZG_CLASS_MOVE_ONLY(EditorSceneView)

public:
	void initialize(bool isActive_);
	void setup(Reference<EditorGizmo> gizmo_, Reference<const EditorHierarchy> hierarchy_);

	void update();
	void draw_scene();
	void draw() override;

	void reset_force();

public:
	void register_world(Reference<RemoteWorldObject> world);

	void create_mesh_instancing(Reference<const RemoteWorldObject> world, const std::string& meshName);
	void register_mesh(Reference<const RemoteWorldObject> world, Reference<const StaticMeshInstance> instance);
	void register_rect(Reference<const RemoteWorldObject> world, Reference<const Rect3d> rect);
	void register_string(Reference<const RemoteWorldObject> world, Reference<const StringRectInstance> stringRect);
	void register_directional_light(Reference<const RemoteWorldObject> world, Reference<const DirectionalLightInstance> lightInstance);
	void write_primitive(Reference<const RemoteWorldObject> world, const std::string& primitiveName, const Affine& affine);

public:
	std::optional<u32> get_layer(Reference<const RemoteWorldObject> world) const;

	Reference<EditorWorldView> get_world_view(Reference<const RemoteWorldObject> worldRef);
	Reference<EditorWorldView> get_current_world_view();

	bool is_hovered_window();
	const Vector2& view_origin() const;
	const Vector2& view_size() const;

	Reference<ImDrawList> draw_list() const;

	Reference<const EditorDebugCamera> query_debug_camera();

private:
	void copy_screen();
	void set_imgui_command();

private:
	bool isHoverWindow{ false };
	bool isActiveGrid{ true };
	u32 layerSize;

	Vector2 origin;
	Vector2 size;

	Reference<EditorGizmo> gizmo;
	Reference<const EditorHierarchy> hierarchy;
	Reference<ImDrawList> drawList;

	std::optional<u32> selectWorldId;

	std::unordered_map<u32, LayerAccessor> worldViews;

	// 描画用データ
	TempTexture screenResultTexture;

	RenderPath renderPath;

	std::unique_ptr<StaticMeshInstance> axisMesh;

	DirectionalLightingExecutor directionalLightingExecutor;
	StaticMeshDrawManager staticMeshDrawManager;
	Rect3dDrawManager rect3dDrawManager;
	StringRectDrawManager stringRectDrawManager;

	std::vector<std::vector<Reference<const DirectionalLightInstance>>> directionalLights;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
