#ifdef DEBUG_FEATURES_ENABLE

#include "RemoteDirectionalLightInstance.h"

using namespace szg;

#include "../../../Window/EditorSceneView.h"

#define COLOR3_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

void RemoteDirectionalLightInstance::setup() {
	RemoteInstanceType::setup();
	debugVisual = std::make_unique<Rect3d>();
	debugVisual->initialize(CVector2::ONE * 0.5f, Vector2{ 0.5f, 0.5f });
	debugVisual->get_material().lightingType = LighingType::None;
	debugVisual->get_material().texture = TextureLibrary::GetTexture("EngineIcon_DirectionalLight.png");

	instance = std::make_unique<DirectionalLightInstance>();

	on_spawn();
	
	sceneView->register_directional_light(query_world(), instance);
	sceneView->register_rect(query_world(), debugVisual);
}

void RemoteDirectionalLightInstance::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	RemoteInstanceType::update_preview(world, parentAffine);

	// ライト方向の線
	Affine affine;
	affine = Affine::FromSRT(
		Vector3{ intensity, intensity, intensity },
		Quaternion::LookForward(direction),
		worldAffine.get_origin()
	);
	sceneView->write_primitive(world, "Line", affine);

	// ライト情報の更新
	instance->light_data_mut().color = color;
	instance->light_data_mut().direction = direction;
	instance->light_data_mut().intensity = intensity;

	instance->get_transform().set_scale(worldAffine.get_basis().to_scale());
	instance->get_transform().set_quaternion(worldAffine.get_basis().to_quaternion());
	instance->get_transform().set_translate(worldAffine.get_origin());
	instance->update_affine();

	// ライトアイコンの更新
	Reference<const EditorDebugCamera> camera = sceneView->query_debug_camera();
	if (camera) {
		debugVisual->look_at(camera);
	}
	debugVisual->get_transform().set_translate(worldAffine.get_origin());
	debugVisual->update_affine();
}

void RemoteDirectionalLightInstance::draw_inspector() {
	ImGui::Text("Type : DirectionalLightInstance");

	hierarchyName.show_gui();
	isUseRuntime.show_gui();

	ImGui::Separator();

	transform.show_gui();

	ImGui::Separator();

	color.show_gui();
	intensity.show_gui();

	ImGui::Text("Direction");
	Vector3 rotate = CVector3::ZERO;
	ImGui::Text(std::format("X : {:.3}, Y : {:.3}, Z : {:.3}", direction.get().x, direction.get().y, direction.get().z).c_str());
	if (ImGui::DragFloat3("World", &rotate.x, 1.0f, -180.0f, 180.0f)) {
		if (rotate.x != 0.0f || rotate.y != 0.0f || rotate.z != 0.0f) {
			direction = direction * Quaternion::EulerDegree(rotate);
		}
	}
	// 向きリセットボタン
	ImGui::Text("Reset");
	if (ImGui::Button("X")) {
		direction = CVector3::BASIS_X;
	}
	ImGui::SameLine();
	if (ImGui::Button("Y")) {
		direction = CVector3::BASIS_Y;
	}
	ImGui::SameLine();
	if (ImGui::Button("Z")) {
		direction = CVector3::BASIS_Z;
	}
	ImGui::SameLine();
	if (ImGui::Button("-X")) {
		direction = -CVector3::BASIS_X;
	}
	ImGui::SameLine();
	if (ImGui::Button("-Y")) {
		direction = -CVector3::BASIS_Y;
	}
	ImGui::SameLine();
	if (ImGui::Button("-Z")) {
		direction = -CVector3::BASIS_Z;
	}

	ImGui::Separator();
	ImGui::Text("Influence Layer");
	for (i32 i = 0; i < sizeof(u32) * 4; ++i) {
		bool isActive = influenceLayer & (1u << i);
		if (isActive) {
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.10f, 0.60f, 0.12f, 1.00f });
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.21f, 0.22f, 0.23f, 0.40f });
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.05f, 0.05f, 0.05f, 0.0f });
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.02f, 0.02f, 0.02f, 1.00f });
		}
		if (ImGui::Button(std::format("{}", i).c_str(), ImVec2{ 25,0 })) {
			influenceLayer = influenceLayer ^ (1u << i);
		}
		if (i % 8 != 7) {
			ImGui::SameLine();
		}
		ImGui::PopStyleColor(2);
	}
}

nlohmann::json RemoteDirectionalLightInstance::serialize() const {
	nlohmann::json result;

	result.update(transform);
	result.update(isUseRuntime);
	result.update(hierarchyName);
	result.update(color);
	result.update(intensity);
	result.update(direction);
	result.update(influenceLayer);
	result["Type"] = instance_type();
	result["Children"] = nlohmann::json::array();
	for (const auto& child : children) {
		result["Children"].emplace_back(child->serialize());
	}

	return result;
}

void RemoteDirectionalLightInstance::on_spawn() {
	auto world = query_world();
	auto result = sceneView->get_layer(world);
	debugVisual->set_layer(result.value_or(-1));
	instance->set_active(true);
}

void RemoteDirectionalLightInstance::on_destroy() {
	debugVisual->set_layer(std::numeric_limits<u32>::max());
	instance->set_active(false);
}

#endif // DEBUG_FEATURES_ENABLE
