#ifdef DEBUG_FEATURES_ENABLE

#include "RemoteAABBColliderInstance.h"

#include "Engine/Debug/Editor/Window/SceneView/EditorSceneView.h"

#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"
#include "Engine/Debug/Editor/RemoteObject/WorldObject/RemoteWorldObject.h"
#include "Engine/Debug/Editor/Window/SceneView/EditorSceneView.h"

using namespace szg;

void RemoteAABBColliderInstance::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	IRemoteInstance<AABBCollider, void*>::update_preview(world, parentAffine);
	Affine primitiveAffine = Affine::FromSRT(size.value_imm(), CQuaternion::IDENTITY, Vector3::Multiply(-offset.value_imm(), size.value_imm()) + worldAffine.get_origin());
	sceneView->write_primitive(world, "Box", primitiveAffine);
}

void RemoteAABBColliderInstance::draw_inspector() {
	ImGui::Text("Type : AABBCollider");

	hierarchyName.show_gui();
	isUseRuntime.show_gui();
	ImGui::Separator();

	transform.show_gui();

	ImGui::Separator();

	auto result = query_world()->collider_group_registry_imm()->combo_gui(colliderId, "Collider Group");
	if (result) {
		EditorValueChangeCommandHandler::GenCommandInstant<i32>(colliderId, result.value());
	}

	ImGui::Separator();
	size.show_gui();
	offset.show_gui();
}

nlohmann::json RemoteAABBColliderInstance::serialize() const {
	nlohmann::json result;

	result.update(transform);
	result.update(isUseRuntime);
	result.update(hierarchyName);
	result.update(size);
	result.update(offset);
	result["Type"] = instance_type();
	result["Children"] = nlohmann::json::array();
	for (const auto& child : children) {
		result["Children"].emplace_back(child->serialize());
	}

	return result;
}

#endif // DEBUG_FEATURES_ENABLE
