#ifdef DEBUG_FEATURES_ENABLE

#include "EditorSceneView.h"

#include <imgui.h>

#include "../../Core/EditorGizmo.h"
#include "../../RemoteObject/RemoteWorldObject.h"
#include "../Hierarchy/EditorHierarchy.h"
#include "Engine/Application/Logger.h"
#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/ScreenTexture.h"
#include "Engine/GraphicsAPI/DirectX/DxSwapChain/DxSwapChain.h"
#include "Engine/Module/Render/RenderPipeline/Debug/Grid/GridPipeline.h"
#include "Engine/Module/Render/RenderPipeline/Debug/PrimitiveLine/PrimitiveLinePipeline.h"
#include "Engine/Module/Render/RenderPipeline/Forward/FontRenderingNode/FontRenderingPipeline.h"
#include "Engine/Module/Render/RenderPipeline/Forward/Mesh/StaticMeshForwardPipeline.h"
#include "Engine/Module/Render/RenderPipeline/Forward/Primitive/Rect3dPipeline.h"
#include "Engine/Module/Render/RenderTargetGroup/SwapChainRenderTargetGroup.h"
#include <Engine/Module/Render/RenderPipeline/Forward/Particle/ParticleBillboardPipeline.h>
#include <Engine/Module/Render/RenderPipeline/Forward/Particle/ParticleMeshPipeline.h>

using namespace szg;

void EditorSceneView::initialize() {
	screenResultTexture.initialize();

	std::vector<std::shared_ptr<BaseRenderPipeline>> nodes;
	std::shared_ptr<StaticMeshForwardPipeline> staticMeshNode = std::make_shared<StaticMeshForwardPipeline>();
	staticMeshNode->initialize();
	nodes.emplace_back(staticMeshNode);

	std::shared_ptr<Rect3dPipeline> rect3dNode = std::make_shared<Rect3dPipeline>();
	rect3dNode->initialize(BlendMode::None);
	nodes.emplace_back(rect3dNode);

	std::shared_ptr<FontRenderingPipeline> stringRectNode = std::make_shared<FontRenderingPipeline>();
	stringRectNode->initialize(BlendMode::None);
	nodes.emplace_back(stringRectNode);

	for (u32 i = 0; i < BLEND_MODE_COUNT; ++i) {
		auto node = std::make_shared<ParticleBillboardPipeline>();
		node->initialize(static_cast<BlendMode>(i));
		nodes.emplace_back(node);
	}

	std::shared_ptr<ParticleMeshPipeline> particleMeshNode = std::make_shared<ParticleMeshPipeline>();
	particleMeshNode->initialize(BlendMode::None);
	nodes.emplace_back(particleMeshNode);

	std::shared_ptr<PrimitiveLinePipeline> primitiveLineNode = std::make_shared<PrimitiveLinePipeline>();
	primitiveLineNode->initialize();
	nodes.emplace_back(primitiveLineNode);

	std::shared_ptr<GridPipeline> gridPipeline = std::make_shared<GridPipeline>();
	gridPipeline->initialize();
	nodes.emplace_back(gridPipeline);

	directionalLightingExecutor.reinitialize(3);
	renderPath.initialize(std::move(nodes));
	directionalLights.resize(32);

	axisMesh = std::make_unique<StaticMeshInstance>("CameraAxis.obj");
	axisMesh->get_materials()[0].lightingType = LighingType::None;

	windowFlags = ImGuiWindowFlags_NoScrollbar;
}

void EditorSceneView::setup(Reference<EditorGizmo> gizmo_, Reference<const EditorHierarchy> hierarchy_) {
	gizmo = gizmo_;
	hierarchy = hierarchy_;
}

void EditorSceneView::update() {
	if (selectWorldId.has_value() && worldViews.contains(selectWorldId.value())) {
		u32 layer = worldViews[selectWorldId.value()].layer;
		EditorWorldView& view = worldViews[selectWorldId.value()].view;

		for (auto& [_, pool] : particlePools) {
			particleUpdaters.update_pool(pool);
		}

		// Windowがフォーカスされている場合のみ更新
		if (is_focus()) {
			view.update();
		}
		view.transfer();
		// デバッグカメラの注視点に描画する
		axisMesh->transform_mut().set_translate(view.get_camera()->view_point());
		axisMesh->set_layer(layer);
		axisMesh->update_affine();
		// カメラが近すぎる場合は非表示にする
		if (view.get_camera()->offset_imm() < 0.1f) {
			axisMesh->set_active(false);
		}
		else {
			axisMesh->set_active(true);
		}

		// リセット
		staticMeshDrawManager.reset_buffer();
		rect3dDrawManager.reset_buffer();
		stringRectDrawManager.reset_buffer();
		directionalLightingExecutor.begin();
		particleBillboardDrawManager.reset_buffer();
		particleMeshDrawManager.reset_buffer();

		// 書き込み
		for (auto& lightInstance : directionalLights[selectWorldId.value()]) {
			if (!lightInstance->is_active()) {
				continue;
			}
			directionalLightingExecutor.write_to_buffer(lightInstance);
		}
		staticMeshDrawManager.transfer();
		rect3dDrawManager.transfer();
		stringRectDrawManager.transfer();

		for (auto [emitter, pool] : particlePools) {
			if (emitter->query_world()->get_id() != selectWorldId.value()) {
				continue;
			}
			pool->sync_draw(particleBillboardDrawManager, particleMeshDrawManager);
		}
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

		// Particle Billboard
		renderPath.next();
		view.register_world_projection(3);
		view.register_world_lighting(4);
		directionalLightingExecutor.set_command(5);
		particleBillboardDrawManager.draw_layer(layer);
		for (u32 i = 0; i < BLEND_MODE_COUNT; ++i) {
			particleBillboardDrawManager.draw_layer_key(layer, static_cast<BlendMode>(i));
			renderPath.next();
		}

		// Particle Mesh
		view.register_world_projection(2);
		view.register_world_lighting(3);
		directionalLightingExecutor.set_command(4);
		particleMeshDrawManager.draw_layer(layer);

		// lines
		renderPath.next();
		view.draw_lines();

		renderPath.next();
		if (isActiveGrid) {
			view.draw_grid();
		}

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
	rect3dDrawManager = Rect3dDrawManager{};
	stringRectDrawManager = StringRectDrawManager{};
	particleBillboardDrawManager = ParticleBillboardDrawManager{};
	particleMeshDrawManager = ParticleMeshDrawManager{};
	particleUpdaters.clear_all();
	particlePools.clear();
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
	rect3dDrawManager.initialize(layerSize);
	stringRectDrawManager.initialize(layerSize);
	particleBillboardDrawManager.initialize(layerSize);
	particleMeshDrawManager.initialize(layerSize);

	staticMeshDrawManager.make_instancing(tmp.layer, "CameraAxis.obj", 1024);
	if (layerSize == 1) {
		staticMeshDrawManager.register_instance(axisMesh);
	}
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

void szg::EditorSceneView::register_particle(Reference<const RemoteWorldObject>, Reference<RemoteEmitterInstance> emitter, Reference<ParticlePool> particle) {
	particlePools.emplace(emitter, particle);
}

void szg::EditorSceneView::unregister_particle(Reference<RemoteEmitterInstance> emitter) {
	particlePools.erase(emitter);
}

void szg::EditorSceneView::create_particle_mesh_instancing(Reference<const RemoteWorldObject> world, const std::string& meshName) {
	if (worldViews.contains(world->get_id())) {
		particleMeshDrawManager.ensure_instancing(worldViews.at(world->get_id()).layer, meshName, BlendMode::None, 1024);
	}
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
	ImGui::Begin("SceneView", &isActive, windowFlags);

	update_focus();

	// Gizmo用ヘッダー描画
	if (ImGui::BeginChild("SceneViewHeader", ImVec2{ 0,30 })) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0], 18);

		gizmo->scene_header();

		ImGui::SameLine();
		ImGui::TextColored(ImColor{ 0.2f, 0.2f, 0.2f }, "|");
		ImGui::SameLine();

		// グリッド表示切替
		if (isActiveGrid) {
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.10f, 0.60f, 0.12f, 1.00f });
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.21f, 0.22f, 0.23f, 0.40f });
			if (ImGui::Button("\ue3ec###GridActive")) {
				isActiveGrid = !isActiveGrid;
			}
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.05f, 0.05f, 0.05f, 0.0f });
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.02f, 0.02f, 0.02f, 1.00f });
			if (ImGui::Button("\ue3eb###GridActive")) {
				isActiveGrid = !isActiveGrid;
			}
		}
		ImGui::PopStyleColor(2);

		ImGui::PopStyleVar(1);
		ImGui::PopFont();
	}
	ImGui::EndChild();

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

#endif // DEBUG_FEATURES_ENABLE
