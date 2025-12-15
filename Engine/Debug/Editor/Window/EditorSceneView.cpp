#ifdef DEBUG_FEATURES_ENABLE

#include "EditorSceneView.h"

using namespace szg;

#include <imgui.h>

#include "../Core/EditorGizmo.h"
#include "../RemoteObject/RemoteWorldObject.h"
#include "EditorHierarchy.h"
#include "Engine/Application/Logger.h"
#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/ScreenTexture.h"
#include "Engine/GraphicsAPI/DirectX/DxSwapChain/DxSwapChain.h"
#include "Engine/Module/Render/RenderPipeline/Debug/PrimitiveLine/PrimitiveLinePipeline.h"
#include "Engine/Module/Render/RenderPipeline/Forward/FontRenderingNode/FontRenderingPipeline.h"
#include "Engine/Module/Render/RenderPipeline/Forward/Mesh/StaticMeshForwardPipeline.h"
#include "Engine/Module/Render/RenderPipeline/Forward/Primitive/Rect3dPipeline.h"
#include "Engine/Module/Render/RenderTargetGroup/SwapChainRenderTargetGroup.h"

void EditorSceneView::initialize(bool isActive_) {
	isActive = isActive_;
	screenResultTexture.initialize();

	std::shared_ptr<StaticMeshForwardPipeline> staticMeshNode = std::make_shared<StaticMeshForwardPipeline>();
	staticMeshNode->initialize();

	std::shared_ptr<Rect3dPipeline> rect3dNode = std::make_shared<Rect3dPipeline>();
	rect3dNode->initialize(BlendMode::None);

	std::shared_ptr<FontRenderingPipeline> stringRectNode = std::make_shared<FontRenderingPipeline>();
	stringRectNode->initialize(BlendMode::None);

	std::shared_ptr<PrimitiveLinePipeline> primitiveLineNode = std::make_shared<PrimitiveLinePipeline>();
	primitiveLineNode->initialize();

	staticMeshDrawManager.initialize(1);
	rect3dDrawManager.initialize(1);
	stringRectDrawManager.initialize(1);
	directionalLightingExecutor.reinitialize(3);
	renderPath.initialize(
		{ staticMeshNode, rect3dNode, stringRectNode, primitiveLineNode }
	);
	directionalLights.resize(32);

	axisMesh = std::make_unique<StaticMeshInstance>("CameraAxis.obj");
	axisMesh->get_materials()[0].lightingType = LighingType::None;

	staticMeshDrawManager.register_instance(axisMesh);
}

void EditorSceneView::setup(Reference<EditorGizmo> gizmo_, Reference<const EditorHierarchy> hierarchy_) {
	gizmo = gizmo_;
	hierarchy = hierarchy_;
}

void EditorSceneView::update() {
	if (selectWorldId.has_value() && worldViews.contains(selectWorldId.value())) {
		EditorWorldView& view = worldViews[selectWorldId.value()].view;
		
		// Windowがフォーカスされている場合のみ更新
		if (is_focus()) {
			view.update();
		}
		view.transfer();
		// デバッグカメラの注視点に描画する
		axisMesh->get_transform().set_translate(view.get_camera()->view_point());
		axisMesh->update_affine();
		// カメラが近すぎる場合は非表示にする
		if (view.get_camera()->offset_imm() > -0.1f) {
			axisMesh->set_active(false);
		}
		else {
			axisMesh->set_active(true);
		}

		directionalLightingExecutor.begin();
		for (auto& lightInstance : directionalLights[selectWorldId.value()]) {
			if (!lightInstance->is_active()) {
				continue;
			}
			directionalLightingExecutor.write_to_buffer(lightInstance);
		}
		staticMeshDrawManager.transfer();
		rect3dDrawManager.transfer();
		stringRectDrawManager.transfer();
	}
}

void EditorSceneView::draw_scene() {
	if (!selectWorldId.has_value()) {
		return;
	}

	if (worldViews.contains(selectWorldId.value())) {
		u32 layer = worldViews[selectWorldId.value()].layer;
		EditorWorldView& view = worldViews[selectWorldId.value()].view;

		// 描画フェーズ
		renderPath.begin();
		Reference<SwapChainRenderTargetGroup> swapChainBuffer = DxSwapChain::GetRenderTarget();
		Reference<DepthStencilTexture> depthStencilTexture = RenderingSystemValues::GetDepthStencilTexture();
		swapChainBuffer->begin_write(true, depthStencilTexture);
		depthStencilTexture->start_write();
		depthStencilTexture->get_as_dsv()->clear();
		auto&& commandList = DxCommand::GetCommandList();
		D3D12_VIEWPORT viewPort{
			0.0f, 0.0f,
			ProjectSettings::ClientSize().x, ProjectSettings::ClientSize().y,
			0.0f, 1.0f
		};
		RECT scissorRect = D3D12_RECT{
			0, 0, (long)ProjectSettings::ClientWidth(), (long)ProjectSettings::ClientHeight()
		};

		// ViewPortの設定
		commandList->RSSetViewports(1, &viewPort);
		// シザー矩形の設定
		commandList->RSSetScissorRects(1, &scissorRect);

		// Mesh
		view.register_world_projection(2);
		view.register_world_lighting(3);
		directionalLightingExecutor.set_command(4);
		staticMeshDrawManager.draw_layer(layer);

		// Rect3d
		renderPath.next();
		view.register_world_projection(3);
		view.register_world_lighting(4);
		directionalLightingExecutor.set_command(5);
		rect3dDrawManager.draw_layer(layer);

		// StringRect
		renderPath.next();
		view.register_world_projection(3);
		stringRectDrawManager.draw_layer(layer);

		// lines
		renderPath.next();
		view.draw_lines();

		renderPath.next();
	}
	copy_screen();
}

void EditorSceneView::draw() {
	set_imgui_command();
}

void EditorSceneView::reset_force() {
	selectWorldId.reset();
	worldViews.clear();
	staticMeshDrawManager = StaticMeshDrawManager{};
	layerSize = 0;
}

void EditorSceneView::register_world(Reference<RemoteWorldObject> world) {
	if (!world) {
		return;
	}
	if (worldViews.contains(world->get_id())) {
		return;
	}
	// 新規にWorldViewを作成
	auto& tmp = worldViews[world->get_id()];
	tmp.view.initialize();
	tmp.view.setup(world);
	tmp.layer = layerSize;
	++layerSize;
	staticMeshDrawManager.initialize(layerSize);
}

void EditorSceneView::create_mesh_instancing(Reference<const RemoteWorldObject> world, const std::string& meshName) {
	if (worldViews.contains(world->get_id())) {
		staticMeshDrawManager.make_instancing(worldViews.at(world->get_id()).layer, meshName, 1024);
	}
}

void EditorSceneView::register_mesh(Reference<const RemoteWorldObject> world, Reference<const StaticMeshInstance> instance) {
	create_mesh_instancing(world, instance->key_id());
	staticMeshDrawManager.register_instance(instance);
}

void EditorSceneView::register_rect(Reference<const RemoteWorldObject> world, Reference<const Rect3d> rect) {
	if (worldViews.contains(world->get_id())) {
		rect3dDrawManager.make_instancing(worldViews.at(world->get_id()).layer, rect->key_id(), 1024);
	}
	rect3dDrawManager.register_instance(rect);
}

void EditorSceneView::register_string(Reference<const RemoteWorldObject> world, Reference<const StringRectInstance> stringRect) {
	if (worldViews.contains(world->get_id())) {
		stringRectDrawManager.make_instancing(worldViews.at(world->get_id()).layer, stringRect->key_id(), 1024);
	}
	stringRectDrawManager.register_instance(stringRect);
}

void EditorSceneView::register_directional_light(Reference<const RemoteWorldObject> world, Reference<const DirectionalLightInstance> lightInstance) {
	if (!worldViews.contains(world->get_id())) {
		return;
	}
	u32 worldId = world->get_id();
	directionalLights[worldId].emplace_back(lightInstance);
}

void EditorSceneView::write_primitive(Reference<const RemoteWorldObject> world, const std::string& primitiveName, const Affine& affine) {
	if (!worldViews.contains(world->get_id())) {
		szgWarning("");
		return;
	}
	worldViews.at(world->get_id()).view.register_primitive(primitiveName, affine);
}

std::optional<u32> EditorSceneView::get_layer(Reference<const RemoteWorldObject> world) const {
	if (!worldViews.contains(world->get_id())) {
		return std::nullopt;
	}
	return worldViews.at(world->get_id()).layer;
}

Reference<EditorWorldView> EditorSceneView::get_world_view(Reference<const RemoteWorldObject> world) {
	if (!worldViews.contains(world->get_id())) {
		return nullptr;
	}
	return worldViews.at(world->get_id()).view;
}

Reference<EditorWorldView> EditorSceneView::get_current_world_view() {
	if (worldViews.empty() || !selectWorldId.has_value() || !worldViews.contains(selectWorldId.value())) {
		return nullptr;
	}
	return worldViews[selectWorldId.value()].view;
}

bool EditorSceneView::is_hovered_window() {
	return
		(isActive && isHoverWindow) ||
		!ImGui::GetIO().WantCaptureMouse;
}

const Vector2& EditorSceneView::view_origin() const {
	return origin;
}

const Vector2& EditorSceneView::view_size() const {
	return size;
}

Reference<ImDrawList> EditorSceneView::draw_list() const {
	return drawList;
}

Reference<const EditorDebugCamera> EditorSceneView::query_debug_camera() {
	if (selectWorldId.has_value() && worldViews.contains(selectWorldId.value())) {
		return worldViews[selectWorldId.value()].view.get_camera();
	}
	return nullptr;
}

void EditorSceneView::copy_screen() {
	auto& command = DxCommand::GetCommandList();
	Reference<ScreenTexture> screen = DxSwapChain::GetWriteBufferTexture();

	screenResultTexture.start_copy_dest();
	screen->start_read();

	command->CopyResource(screenResultTexture.get_resource().Get(), screen->get_resource().Get());
}

void EditorSceneView::set_imgui_command() {
	if (!isActive) {
		return;
	}
	if (isHoverWindow && (ImGui::GetIO().MouseDown[1] || ImGui::GetIO().MouseDown[2])) {
		ImGui::SetNextWindowFocus();
	}

	screenResultTexture.start_read();
	ImGui::Begin("Scene", &isActive, ImGuiWindowFlags_NoScrollbar);

	update_focus();

	// Gizmo用ヘッダー描画
	gizmo->scene_header();
	ImGui::Separator();
	ImGui::BeginTabBar("WorldViewTabBar", ImGuiTabBarFlags_DrawSelectedOverline);

	// Guizmoのために必要
	drawList = ImGui::GetWindowDrawList();
	isHoverWindow = ImGui::IsWindowHovered();

	// 各WorldViewをImGuiに描画
	auto& worldList = hierarchy->world_list();
	for (u32 i = 0; i < worldList.size(); ++i) {
		u32 world = worldList[i]->get_id();
		if (worldViews.contains(world)) {
			auto [result, pos, size_] = worldViews.at(world).view.draw_editor(screenResultTexture);
			if (result) {
				selectWorldId = world;
				origin = pos;
				size = size_;
			}
		}
	}

	ImGui::EndTabBar();
	ImGui::End();
}

#endif // DEBUG_FEATURES_ENABLE
