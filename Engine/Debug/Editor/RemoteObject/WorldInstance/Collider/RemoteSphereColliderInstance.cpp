#ifdef DEBUG_FEATURES_ENABLE

#include "RemoteSphereColliderInstance.h"

using namespace szg;

#include "../../../Window/EditorSceneView.h"

void RemoteSphereColliderInstance::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	IRemoteInstance<SphereCollider, void*>::update_preview(world, parentAffine);
	Affine primitiveAffine = Affine::FromScale(Vector3{ radius.cget(), radius.cget(), radius.cget() }) * Affine::FromTranslate(worldAffine.get_origin());
	sceneView->write_primitive(world, "Sphere", primitiveAffine);
}

void RemoteSphereColliderInstance::draw_inspector() {
	ImGui::Text("Type : SphereCollider");

	hierarchyName.show_gui();
	isUseRuntime.show_gui();
	ImGui::Separator();

	transform.show_gui();

	ImGui::Separator();
	radius.show_gui();
}

nlohmann::json RemoteSphereColliderInstance::serialize() const {
	nlohmann::json result;

	result.update(transform);
	result.update(isUseRuntime);
	result.update(hierarchyName);
	result.update(radius);
	result["Type"] = instance_type();
	result["Children"] = nlohmann::json::array();
	for (const auto& child : children) {
		result["Children"].emplace_back(child->serialize());
	}

	return result;
}

#endif // DEBUG_FEATURES_ENABLE
