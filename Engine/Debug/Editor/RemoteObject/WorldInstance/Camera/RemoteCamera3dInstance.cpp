#ifdef DEBUG_FEATURES_ENABLE

#include "RemoteCamera3dInstance.h"

using namespace szg;

#include "../../../Window/EditorSceneView.h"
#include "./DebugProjectionVariantVisitor.h"
#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"

void RemoteCamera3dInstance::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	IRemoteInstance<CameraInstance, void*>::update_preview(world, parentAffine);
	sceneView->write_primitive(world, "Frustum", worldAffine);
}

void RemoteCamera3dInstance::draw_inspector() {
	ImGui::Text("Type : CameraInstance");

	hierarchyName.show_gui();
	isUseRuntime.show_gui();
	ImGui::Separator();

	transform.show_gui();

	ImGui::Separator();

	ImGui::Text("Projection Type");
	if (ImGui::RadioButton("Perspective", std::holds_alternative<PerspectiveParameters>(projectionParameters))) {
		EditorValueChangeCommandHandler::GenCommand<decltype(projectionParameters)>(projectionParameters);
		projectionParameters = PerspectiveParameters{};
		EditorValueChangeCommandHandler::End();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Orthographic", std::holds_alternative<OrthroParameters>(projectionParameters))) {
		EditorValueChangeCommandHandler::GenCommand<decltype(projectionParameters)>(projectionParameters);
		projectionParameters = OrthroParameters{};
		EditorValueChangeCommandHandler::End();
	}

	std::visit(DebugProjectionVariantVisitor{}, projectionParameters);
}

nlohmann::json RemoteCamera3dInstance::serialize() const {
	nlohmann::json result;

	result.update(transform);
	result.update(isUseRuntime);
	result.update(hierarchyName);

	{
		nlohmann::json projectionJson = std::visit(DebugProjectionVariantSaver{}, projectionParameters);
		result["Projection"] = projectionJson;
	}

	result["Type"] = instance_type();
	result["Children"] = nlohmann::json::array();
	for (const auto& child : children) {
		result["Children"].emplace_back(child->serialize());
	}

	return result;
}

#endif // DEBUG_FEATURES_ENABLE
