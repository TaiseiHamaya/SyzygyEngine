#include "RemoteRect3dInstance.h"

#ifdef DEBUG_FEATURES_ENABLE

using namespace szg;

#include "../../../Window/EditorSceneView.h"

#define COLOR_RGBA_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

void RemoteRect3dInstance::setup() {
	debugVisual = std::make_unique<Rect3d>();
	on_spawn();
	debugVisual->initialize(size, pivot);
	debugVisual->set_draw(true);
	debugVisual->keyID = BlendMode::None;
	// Editor側でDrawExecutorに登録
	if (sceneView) {
		// TODO: SceneViewでRectを登録できるようにする
		sceneView->register_rect(query_world(), debugVisual);
	}

	IRemoteInstance<Rect3d, Rect3d>::setup();
}

void RemoteRect3dInstance::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	IRemoteInstance<Rect3d, Rect3d>::update_preview(world, parentAffine);

	debugVisual->localAffine = worldAffine;
	debugVisual->data.size = size;
	debugVisual->data.pivot = pivot;
	debugVisual->isDraw = isDraw;
	debugVisual->keyID = BlendMode::None;

	PrimitiveMaterial& dest = debugVisual->get_material();
	dest.texture = TextureLibrary::GetTexture(material.texture);
	dest.color = material.color;
	dest.uvTransform.copy(material.uvTransform);
	dest.lightingType = material.lightingType;
	dest.shininess = material.shininess;
}

void RemoteRect3dInstance::draw_inspector() {
	ImGui::Text("Type : Rect3dInstance");

	hierarchyName.show_gui();
	isUseRuntime.show_gui();

	ImGui::Separator();

	transform.show_gui();

	ImGui::Separator();

	isDraw.show_gui();
	layer.show_gui();

	ImGui::Separator();

	size.show_gui();
	pivot.show_gui();
	isFlipY.show_gui();

	ImGui::Separator();
	ImGui::Text("Material");
	if (ImGui::Button("ResetMaterialData")) {
		reset_material();
	}
	{
		std::string cache = material.texture;
		auto result = TextureLibrary::TextureListGui(cache);
		if (result) {
			EditorValueChangeCommandHandler::GenCommand<std::string>(material.texture);
			material.texture = cache;
			EditorValueChangeCommandHandler::End();
		}
	}
	material.color.show_gui();
	material.uvTransform.show_gui();
	{
		std::optional<LighingType> temp;
		if (ImGui::RadioButton("None", material.lightingType == LighingType::None)) {
			if (material.lightingType != LighingType::None) {
				temp = LighingType::None;
			}
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Lambert", material.lightingType == LighingType::Lambert)) {
			if (material.lightingType != LighingType::Lambert) {
				temp = LighingType::Lambert;
			}
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Half lambert", material.lightingType == LighingType::HalfLambert)) {
			if (material.lightingType != LighingType::HalfLambert) {
				temp = LighingType::HalfLambert;
			}
		}

		if (temp.has_value()) {
			EditorValueChangeCommandHandler::GenCommand<LighingType>(material.lightingType);
			material.lightingType = temp.value();
			EditorValueChangeCommandHandler::End();
		}
	}

	material.shininess.show_gui();
}

nlohmann::json RemoteRect3dInstance::serialize() const {
	nlohmann::json result;
	result.update(transform);
	result.update(isUseRuntime);
	result.update(hierarchyName);

	result["Type"] = instance_type();

	result.update(isDraw);
	result.update(layer);

	result.update(size);
	result.update(pivot);
	result.update(isFlipY);

	nlohmann::json materialJson;
	materialJson["Texture"] = material.texture;
	materialJson.update(material.color);
	materialJson.update(material.uvTransform);
	materialJson["LightingType"] = material.lightingType;
	materialJson.update(material.shininess);
	result["Material"] = materialJson;

	result["Children"] = nlohmann::json::array();
	for (const auto& child : children) {
		result["Children"].emplace_back(child->serialize());
	}
	return result;
}

void RemoteRect3dInstance::on_spawn() {
	auto world = query_world();
	auto result = sceneView->get_layer(world);
	debugVisual->set_layer(result.value_or(-1));
}

void RemoteRect3dInstance::on_destroy() {
	debugVisual->set_layer(std::numeric_limits<u32>::max());
}

void RemoteRect3dInstance::reset_material() {
	EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());

	EditorValueChangeCommandHandler::GenCommand<std::string>(material.texture);
	material.texture = "Error.png";
	EditorValueChangeCommandHandler::End();
	EditorValueChangeCommandHandler::GenCommand<ColorRGBA>(material.color.value_mut());
	material.color = CColorRGBA::WHITE;
	EditorValueChangeCommandHandler::End();
	material.uvTransform.set(Transform2D{});
	EditorValueChangeCommandHandler::GenCommand<r32>(material.shininess.value_mut());
	material.shininess = 50.0f;
	EditorValueChangeCommandHandler::End();

	EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
}

#endif // DEBUG_FEATURES_ENABLE
