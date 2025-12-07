#ifdef DEBUG_FEATURES_ENABLE

#include "RemoteCamera3dInstance.h"

using namespace szg;

#include "../../../Window/EditorSceneView.h"

void RemoteCamera3dInstance::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	IRemoteInstance<Camera3D, void*>::update_preview(world, parentAffine);
	sceneView->write_primitive(world, "Frustum", worldAffine);
}

void RemoteCamera3dInstance::draw_inspector() {
	ImGui::Text("Type : Camera3D");

	hierarchyName.show_gui();
	isUseRuntime.show_gui();
	ImGui::Separator();

	transform.show_gui();

	ImGui::Separator();
	fovY.show_gui();
	aspectRatio.show_gui();
	nearClip.show_gui();
	farClip.show_gui();
}

nlohmann::json RemoteCamera3dInstance::serialize() const {
	nlohmann::json result;

	result.update(transform);
	result.update(isUseRuntime);
	result.update(hierarchyName);
	result.update(fovY);
	result.update(aspectRatio);
	result.update(nearClip);
	result.update(farClip);
	result["Type"] = instance_type();
	result["Children"] = nlohmann::json::array();
	for (const auto& child : children) {
		result["Children"].emplace_back(child->serialize());
	}

	return result;
}

#endif // DEBUG_FEATURES_ENABLE
