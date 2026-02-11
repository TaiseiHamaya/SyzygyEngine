#ifdef DEBUG_FEATURES_ENABLE

#include "RemoteStringRectInstance.h"

using namespace szg;

#include "../../../Window/EditorSceneView.h"

#define COLOR_RGBA_SERIALIZER
#define VECTOR2_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

void RemoteStringRectInstance::setup() {
	debugVisual = std::make_unique<StringRectInstance>();
	on_spawn();
	debugVisual->initialize(font, fontSize, pivot);
	debugVisual->set_draw(true);
	debugVisual->keyID = BlendMode::None;
	// Editor側でDrawExecutorに登録
	if (sceneView) {
		sceneView->register_string(query_world(), debugVisual);
	}

	IRemoteInstance<StringRectInstance, StringRectInstance>::setup();
}

void RemoteStringRectInstance::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	IRemoteInstance<StringRectInstance, StringRectInstance>::update_preview(world, parentAffine);

	debugVisual->localAffine = worldAffine;
	debugVisual->isDraw = isDraw;
	debugVisual->keyID = BlendMode::None;

	debugVisual->reset_string(text.value_imm());
	debugVisual->get_material().color = color;
	if (isChangedValue) {
		debugVisual->data.pivot = pivot;
		debugVisual->data.fontSize = fontSize;
		debugVisual->set_font(font);
	}
}

void RemoteStringRectInstance::draw_inspector() {
	ImGui::Text("Type : StringRectInstance");

	hierarchyName.show_gui();
	isUseRuntime.show_gui();

	ImGui::Separator();

	transform.show_gui();

	ImGui::Separator();

	isDraw.show_gui();
	layer.show_gui();

	ImGui::Separator();

	isChangedValue = false;
	if (font.show_gui().any()) {
		isChangedValue = true;
	}

	isChangedValue |= fontSize.show_gui().any();
	isChangedValue |= pivot.show_gui().any();
	isChangedValue |= text.show_gui().any();
	color.show_gui();
}

nlohmann::json RemoteStringRectInstance::serialize() const {
	nlohmann::json result;

	result.update(transform);
	result.update(isUseRuntime);
	result.update(hierarchyName);

	result["Type"] = instance_type();

	result.update(isDraw);
	result.update(layer);

	result.update(fontSize);
	result.update(pivot);
	result.update(text);
	result.update(color);
	result.update(font);

	result["Children"] = nlohmann::json::array();
	for (const auto& child : children) {
		result["Children"].emplace_back(child->serialize());
	}
	return result;
}

void RemoteStringRectInstance::on_spawn() {
	auto world = query_world();
	auto result = sceneView->get_layer(world);
	debugVisual->set_layer(result.value_or(-1));
}

void RemoteStringRectInstance::on_destroy() {
	debugVisual->set_layer(std::numeric_limits<u32>::max());
}

#endif // DEBUG_FEATURES_ENABLE
