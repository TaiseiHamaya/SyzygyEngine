#ifdef DEBUG_FEATURES_ENABLE

#include "EditorDebugCamera.h"

using namespace szg;

#include <Library/Math/VectorConverter.h>

#include "Engine/Application/ProjectSettings/ProjectSettings.h"

#include <imgui.h>

void EditorDebugCamera::initialize() {
	set_perspective_fov_info(
		0.45f,
		(float)ProjectSettings::ClientWidth() / ProjectSettings::ClientHeight(),
		0.1f, 1000
	);

	constraint = std::make_unique<WorldInstance>();

	this->reparent(constraint);
}

void EditorDebugCamera::update() {
	// マウスの移動量を取得
	Vector2 mouseDelta = {
		ImGui::GetIO().MouseDelta.x,
		ImGui::GetIO().MouseDelta.y
	};

	// 注視距離設定
	r32 wheel = static_cast<r32>(ImGui::GetIO().MouseWheel);
	if (wheel != 0) {
		wheel = wheel / std::abs(wheel); // 正規化
		if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
			wheel *= 10.0f; // シフト押下で倍率をかける
		}
		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
			// constraintを前後移動
			constraint->transform_mut().plus_translate(Vector3{ 0,0,wheel } * transform.get_quaternion());
		}
		else {
			// 注視距離を変更
			offset = std::max(offset - wheel, 0.0f);
		}
	}

	// 右クリック(回転)
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
		// 倍率をかけて調整
		Vector2 rotateAngle = mouseDelta / 200;
		Quaternion rotation = transform.get_quaternion();
		// 平行成分と垂直成分でQuaternionを生成
		Quaternion horizontal = Quaternion::AngleAxis(CVector3::BASIS_Y, rotateAngle.x);
		Quaternion vertical = Quaternion::AngleAxis(CVector3::BASIS_X, rotateAngle.y);
		// 垂直->元->平行で適用させる
		transform.set_quaternion(horizontal * rotation * vertical);
	}
	// 中クリック(Translate)
	else if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
		// Vector3にし、倍率をかける
		Vector3 move = Converter::ToVector3(mouseDelta / 100, 0);
		if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
			move *= 10.0f;
		}
		// X軸は反転させる
		move.x *= -1;
		// デバッグカメラの方向を向かせる
		constraint->transform_mut().plus_translate(move * transform.get_quaternion());
	}
	Vector3 offsetVec3 = Vector3{ 0,0,-offset };
	transform.set_translate(offsetVec3 * transform.get_quaternion());
}

void EditorDebugCamera::update_affine() {
	CameraInstance::update_affine();
}

Vector3 szg::EditorDebugCamera::view_point() const {
	return constraint->world_position();
}

r32 szg::EditorDebugCamera::offset_imm() const {
	return offset;
}

Reference<WorldInstance> szg::EditorDebugCamera::constraint_mut() noexcept {
	return constraint;
}

#endif // DEBUG_FEATURES_ENABLE
