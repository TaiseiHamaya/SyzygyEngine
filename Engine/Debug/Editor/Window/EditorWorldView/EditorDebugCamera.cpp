#ifdef DEBUG_FEATURES_ENABLE

#include "EditorDebugCamera.h"

using namespace szg;

#include <Library/Math/VectorConverter.h>

#include "../EditorSceneView.h"
#include "Engine/Application/ProjectSettings/ProjectSettings.h"

void EditorDebugCamera::initialize() {
	set_perspective_fov_info(
		0.45f,
		(float)ProjectSettings::ClientWidth() / ProjectSettings::ClientHeight(),
		0.1f, 1000
	);

	constraint = std::make_unique<StaticMeshInstance>("CameraAxis.obj");
	constraint->get_materials()[0].lightingType = LighingType::None;

	this->reparent(constraint);
	mouseInputHandler.initialize({ MouseID::Middle, MouseID::Right });
}

void EditorDebugCamera::update() {
	mouseInputHandler.update();

	if (!sceneView || !sceneView->is_hovered_window()) {
		return;
	}

	// マウスの移動量を取得
	Vector2 mouseDelta = Input::MouseDelta();
	
	// 注視距離設定
	r32 wheel = static_cast<r32>(Input::WheelDelta());
	offset = std::min(offset + wheel, 0.0f);

	// 右クリック(回転)
	if (mouseInputHandler.press(MouseID::Right)) {
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
	else if (mouseInputHandler.press(MouseID::Middle)) {
		// Vector3にし、倍率をかける
		Vector3 move = Converter::ToVector3(mouseDelta / 100, 0);
		// X軸は反転させる
		move.x *= -1;
		// デバッグカメラの方向を向かせる
		constraint->get_transform().plus_translate(move * transform.get_quaternion());
	}
	transform.set_translate(Vector3{ 0,0,offset } *transform.get_quaternion());

	if (offset < -0.001) {
		constraint->set_draw(true);
	}
	else {
		constraint->set_draw(false);
	}
}

void EditorDebugCamera::update_affine() {
	constraint->update_affine();
	Camera3D::update_affine();
}

void EditorDebugCamera::Setup(Reference<EditorSceneView> sceneView_) {
	sceneView = sceneView_;
}

#endif // DEBUG_FEATURES_ENABLE
