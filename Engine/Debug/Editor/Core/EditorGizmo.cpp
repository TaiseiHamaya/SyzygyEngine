#ifdef DEBUG_FEATURES_ENABLE

#include "EditorGizmo.h"

using namespace szg;

#include <imgui.h>

#include "./EditorSelectObject.h"

#include <Library/Math/Affine.h>
#include <Library/Math/Transform3D.h>

#include "../Command/EditorValueChangeCommandHandler.h"
#include "../RemoteObject/IRemoteObject.h"
#include "../RemoteObject/RemoteWorldObject.h"
#include "../Window/EditorWorldView/EditorWorldView.h"
#include "Engine/Module/World/Camera/Camera3D.h"

EditorGizmo::EditorGizmo() {
	gizmoKeyHandler.initialize({ KeyID::LControl });
}
EditorGizmo::~EditorGizmo() = default;

void EditorGizmo::begin_frame(const Vector2& origin, const Vector2& size) {
	ImGuizmo::SetRect(origin.x, origin.y, size.x, size.y);
}

void EditorGizmo::draw_gizmo(Reference<EditorSelectObject> select, Reference<const EditorWorldView> world) {
	if (!select || !world) {
		return;
	}

	// get select
	auto& item = select->get_item_mut();

	// null check
	if (!item.object || !item.transformData.affine || !item.transformData.transform) {
		return;
	}

	// RootWorld check
	{
		bool check = false;
		Reference<IRemoteObject> remoteObject = item.object;
		while (remoteObject) {
			if (remoteObject == world->remote_world()) {
				check = true;
				break;
			}
			remoteObject = remoteObject->get_parent();
		}
		if (!check) {
			// RootWorldが違う
			return;
		}
	}

	// SetCamera
	{
		Reference<const EditorDebugCamera> camera = world->get_camera();
		view = camera->view_affine().to_matrix();
		proj = camera->proj_matrix();
	}

	// to matrix
	Matrix4x4 matrix = item.transformData.affine->to_matrix();
	// create parent affine
	Affine parentAffine =
		Affine::FromTransform3D(*item.transformData.transform).inverse() * *item.transformData.affine;

	gizmoKeyHandler.update();
	std::array<float, 3> snap = { 0.0f, 0.0f, 0.0f };
	switch (operation) {
	case ImGuizmo::OPERATION::SCALEU:
		snap = { 0.1f, 0.1f, 0.1f };
		break;
	case ImGuizmo::OPERATION::ROTATE:
		snap = { 15.0f, 15.0f, 15.0f };
		break;
	case ImGuizmo::OPERATION::TRANSLATE:
		snap = { 1.0f, 1.0f, 1.0f };
		break;
	default:
		break;
	}
	// manipulate
	gizmoState <<= 1;
	ImGuizmo::Manipulate(&view[0][0], &proj[0][0], operation, mode, &matrix[0][0], nullptr, gizmoKeyHandler.press(KeyID::LControl) ? snap.data() : nullptr);
	gizmoState.set(0, ImGuizmo::IsUsing());
	if (gizmoState == 0b01) {
		switch (operation) {
		case ImGuizmo::OPERATION::SCALEU:
			EditorValueChangeCommandHandler::GenCommand<Vector3>(item.transformData.transform->get_scale());
			break;
		case ImGuizmo::OPERATION::ROTATE:
			EditorValueChangeCommandHandler::GenCommand<Quaternion>(item.transformData.transform->get_quaternion());
			break;
		case ImGuizmo::OPERATION::TRANSLATE:
			EditorValueChangeCommandHandler::GenCommand<Vector3>(item.transformData.transform->get_translate());
			break;
		default:
			break;
		}
	}

	if (gizmoState.any()) {
		// to affine
		Affine decomposed = Affine::FromMatrix(matrix);
		// ローカル座標系に戻す
		Affine affine = decomposed * parentAffine.inverse();

		// decompose
		item.transformData.transform->set_scale(affine.get_basis().to_scale());
		item.transformData.transform->set_quaternion(affine.get_basis().to_quaternion());
		item.transformData.transform->set_translate(affine.get_origin());
	}
	if (gizmoState == 0b10) {
		EditorValueChangeCommandHandler::End();
	}
}

void EditorGizmo::scene_header() {
	// SRTのボタン
	std::array<string_literal, 3> labels = { "\uf655", "\ue577", "\uf393" };
	std::array<ImGuizmo::OPERATION, 3> operations = {
		ImGuizmo::OPERATION::SCALEU,
		ImGuizmo::OPERATION::ROTATE,
		ImGuizmo::OPERATION::TRANSLATE
	};
	for (u32 i = 0; i < 3; ++i) {
		if (operation == operations[i]) {
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.10f, 0.60f, 0.12f, 1.00f });
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.21f, 0.22f, 0.23f, 0.40f });
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.05f, 0.05f, 0.05f, 0.0f });
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.02f, 0.02f, 0.02f, 1.00f });
		}
		if (ImGui::Button(std::format("{}##SRTButton", labels[i]).c_str())) {
			operation = operations[i];
		}
		ImGui::PopStyleColor(2);
		ImGui::SameLine();
	}

	ImGui::TextColored(ImColor{ 0.2f, 0.2f, 0.2f }, "|");
	ImGui::SameLine();

	// グローバル/ローカル切り替えボタン
	std::string tooptip;
	if (mode == ImGuizmo::MODE::WORLD) {
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.10f, 0.60f, 0.12f, 1.00f });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.21f, 0.22f, 0.23f, 0.40f });
		if (ImGui::Button("\ue64c")) {
			mode = ImGuizmo::MODE::LOCAL;
		}
		tooptip = "World transform";
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.05f, 0.05f, 0.05f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.02f, 0.02f, 0.02f, 1.00f });
		if (ImGui::Button("\uf1ca")) {
			mode = ImGuizmo::MODE::WORLD;
		}
		tooptip = "Local transform";
	}
	ImGui::PopStyleColor(2);
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(tooptip.c_str());
	}
}

#endif // DEBUG_FEATURES_ENABLE
