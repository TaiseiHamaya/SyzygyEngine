#ifdef DEBUG_FEATURES_ENABLE

#include "RemotePointLightInstance.h"

using namespace szg;

#include "../../../Window/EditorSceneView.h"

#define COLOR_RGB_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

void RemotePointLightInstance::setup() {
	RemoteInstanceType::setup();
	debugVisual = std::make_unique<Rect3d>();
	debugVisual->initialize(CVector2::HALF, CVector2::HALF);
	debugVisual->get_material().lightingType = LighingType::None;
	debugVisual->get_material().texture = TextureLibrary::GetTexture("EngineIcon_DirectionalLight.png");

	on_spawn();

	sceneView->register_rect(query_world(), debugVisual);
}

void RemotePointLightInstance::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	RemoteInstanceType::update_preview(world, parentAffine);

	Affine primitiveAffine = Affine::FromScale(Vector3{ radius.value_imm(), radius.value_imm(), radius.value_imm() }) * Affine::FromTranslate(worldAffine.get_origin());
	sceneView->write_primitive(world, "Sphere", primitiveAffine);

	Reference<const EditorDebugCamera> camera = sceneView->query_debug_camera();
	if (camera) {
		debugVisual->look_at(camera);
	}
	debugVisual->transform_mut().set_translate(worldAffine.get_origin());
	debugVisual->update_affine();
}

void RemotePointLightInstance::draw_inspector() {
	ImGui::Text("Type : PointLight");

	hierarchyName.show_gui();
	isUseRuntime.show_gui();
	ImGui::Separator();

	transform.show_gui();

	ImGui::Separator();
	color.show_gui();
	intensity.show_gui();
	radius.show_gui();
	decay.show_gui();

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

nlohmann::json RemotePointLightInstance::serialize() const {
	nlohmann::json result;

	result.update(transform);
	result.update(isUseRuntime);
	result.update(hierarchyName);
	result.update(color);
	result.update(intensity);
	result.update(radius);
	result.update(decay);
	result.update(influenceLayer);
	result["Type"] = instance_type();
	result["Children"] = nlohmann::json::array();
	for (const auto& child : children) {
		result["Children"].emplace_back(child->serialize());
	}

	return result;
}

void RemotePointLightInstance::on_spawn() {
	auto world = query_world();
	auto result = sceneView->get_layer(world);
	debugVisual->set_layer(result.value_or(-1));
}

void RemotePointLightInstance::on_destroy() {
	debugVisual->set_layer(std::numeric_limits<u32>::max());
}

#endif // DEBUG_FEATURES_ENABLE
