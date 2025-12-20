#ifdef DEBUG_FEATURES_ENABLE

#include "EditorHierarchy.h"

using namespace szg;

#include <imgui.h>
#include <imgui_stdlib.h>

#include "../Adapter/EditorSceneSerializer.h"
#include "./EditorSceneView.h"
#include "../EditorMain.h"
#include "../Command/EditorCommandInvoker.h"
#include "../Command/EditorCreateObjectCommand.h"
#include "../Command/EditorDeleteObjectCommand.h"
#include "../RemoteObject/FolderObject.h"
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
#include "Engine/Assets/Json/JsonAsset.h"

void EditorHierarchy::setup(Reference<EditorSelectObject> select_, Reference<EditorSceneView> sceneView_) {
	select = select_;
	sceneView = sceneView_;
}

void EditorHierarchy::finalize() {
	scene.reset();
}

void EditorHierarchy::update_preview() {
	scene->update_preview(nullptr, nullptr);
}

void EditorHierarchy::load(const std::string& sceneName) {
	isActive = true;

	scene = EditorSceneSerializer::CreateRemoteScene(sceneName);

	scene->setup();
}

void EditorHierarchy::save(const std::filesystem::path& path) const {
	for (auto& world : scene->get_remote_worlds()) {
		JsonAsset worldJson{ path / "Worlds" / (world->world_name() + ".json") };
		worldJson.get().clear();
		worldJson.get() = world->serialize();
		worldJson.save();
	}
}

void EditorHierarchy::draw() {
	if (!isActive) {
		return;
	}

	int flags = 0;
	ImGui::Begin("Hierarchy", &isActive, flags);

	update_focus();

	// 検索ボックス
	ImGui::InputText("##HierarchySearch", &searchString); ImGui::SameLine();
	if (ImGui::Button("\ue5cd")) {
		searchString.clear();
	}

	ImVec2 tempPos = ImGui::GetCursorPos();

	if (searchString.empty()) {
		ImGui::SetCursorPos({ 16, 30 });
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));
		ImGui::Text("Search");
		ImGui::PopStyleColor();
	}

	ImGui::SetCursorPos(tempPos);
	scene->draw_hierarchy(select);

	// 右クリックメニュー
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsWindowFocused()) {
		ImGui::OpenPopup("HierarchyMenu");
	}

	if (ImGui::BeginPopup("HierarchyMenu")) {
		//Instance作成
		if (ImGui::BeginMenu("CreateInstance")) {
			ImGui::InputText("##MenuSearch", &menuString); ImGui::SameLine();
			if (ImGui::Button("\ue5cd")) {
				menuString.clear();
			}
			ImGui::SeparatorText("Instance");
			if (ImGui::MenuItem("WorldInstance")) {
				if (select->get_item_mut().object) {
					EditorCommandInvoker::Execute(
						std::make_unique<EditorCreateObjectCommand>(
							select->get_item_mut().object,
							std::make_unique<RemoteWorldInstance>()
						)
					);
				}
			}
			ImGui::SeparatorText("Rendering");
			if (ImGui::MenuItem("StaticMeshInstance")) {
				if (select->get_item_mut().object) {
					EditorCommandInvoker::Execute(
						std::make_unique<EditorCreateObjectCommand>(
							select->get_item_mut().object,
							std::make_unique<RemoteStaticMeshInstance>()
						)
					);
				}
			}
			if (ImGui::MenuItem("SkinningMeshInstance")) {
				if (select->get_item_mut().object) {
					EditorCommandInvoker::Execute(
						std::make_unique<EditorCreateObjectCommand>(
							select->get_item_mut().object,
							std::make_unique<RemoteSkinningMeshInstance>()
						)
					);
				}
			}
			if (ImGui::MenuItem("Rect3dInstance")) {
				if (select->get_item_mut().object) {
					EditorCommandInvoker::Execute(
						std::make_unique<EditorCreateObjectCommand>(
							select->get_item_mut().object,
							std::make_unique<RemoteRect3dInstance>()
						)
					);
				}
			}
			if (ImGui::MenuItem("StringRectInstance")) {
				if (select->get_item_mut().object) {
					EditorCommandInvoker::Execute(
						std::make_unique<EditorCreateObjectCommand>(
							select->get_item_mut().object,
							std::make_unique<RemoteStringRectInstance>()
						)
					);
				}
			}

			ImGui::SeparatorText("Camera");
			if (ImGui::MenuItem("Camera3D")) {
				if (select->get_item_mut().object) {
					EditorCommandInvoker::Execute(
						std::make_unique<EditorCreateObjectCommand>(
							select->get_item_mut().object,
							std::make_unique<RemoteCamera3dInstance>()
						)
					);
				}
			}

			ImGui::SeparatorText("Light");
			if (ImGui::MenuItem("DirectionalLightInstance")) {
				if (select->get_item_mut().object) {
					EditorCommandInvoker::Execute(
						std::make_unique<EditorCreateObjectCommand>(
							select->get_item_mut().object,
							std::make_unique<RemoteDirectionalLightInstance>()
						)
					);
				}
			}
			if (ImGui::MenuItem("PointLightInstance")) {
				if (select->get_item_mut().object) {
					EditorCommandInvoker::Execute(
						std::make_unique<EditorCreateObjectCommand>(
							select->get_item_mut().object,
							std::make_unique<RemotePointLightInstance>()
						)
					);
				}
			}


			ImGui::SeparatorText("Collider");
			if (ImGui::MenuItem("AABBColliderInstance")) {
				if (select->get_item_mut().object) {
					EditorCommandInvoker::Execute(
						std::make_unique<EditorCreateObjectCommand>(
							select->get_item_mut().object,
							std::make_unique<RemoteAABBColliderInstance>()
						)
					);
				}
			}
			if (ImGui::MenuItem("SphereColliderInstance")) {
				if (select->get_item_mut().object) {
					EditorCommandInvoker::Execute(
						std::make_unique<EditorCreateObjectCommand>(
							select->get_item_mut().object,
							std::make_unique<RemoteSphereColliderInstance>()
						)
					);
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("CreateWorld")) {
			EditorCommandInvoker::Execute(
				std::make_unique<EditorCreateObjectCommand>(
					scene,
					std::make_unique<RemoteWorldObject>()
				)
			);
		}

		// Folder作成
		if (ImGui::MenuItem("CreateFolder")) {
			if (select->get_item_mut().object) {
				EditorCommandInvoker::Execute(
					std::make_unique<EditorCreateObjectCommand>(
						select->get_item_mut().object,
						std::make_unique<FolderObject>()
					)
				);
			}
		}

		if (ImGui::MenuItem("Delete")) {
			if (select->get_item_mut().object && select->get_item_mut().object.ptr() != scene.get()) {
				EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());
				EditorCommandInvoker::Execute(std::make_unique<EditorDeleteObjectCommand>(select->get_item_mut().object));
				EditorCommandInvoker::Execute(std::make_unique<EditorSelectCommand>(nullptr));
				EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
			}
		}

		if (ImGui::MenuItem("SaveScene")) {
			EditorMain::SeveScene();
		}

		ImGui::EndPopup();
	}

	ImGui::End();
}

std::string EditorHierarchy::current_scene_name() const {
	return scene->name();
}

const std::vector<std::unique_ptr<RemoteWorldObject>>& EditorHierarchy::world_list() const {
	return scene->get_remote_worlds();
}

Reference<const RemoteSceneObject> EditorHierarchy::scene_imm() const {
	return scene;
}

#endif // DEBUG_FEATURES_ENABLE
