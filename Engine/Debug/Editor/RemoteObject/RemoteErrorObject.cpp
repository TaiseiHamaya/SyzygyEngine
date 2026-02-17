#ifdef DEBUG_FEATURES_ENABLE

#include "RemoteErrorObject.h"

#include <format>

#include <imgui.h>

#include "Engine/Application/Logger.h"

#include "../Command/EditorCommandInvoker.h"
#include "../Command/EditorSelectCommand.h"

using namespace szg;

RemoteErrorObject::RemoteErrorObject(const std::string& msg) :
	errorMessage(msg) {
	szgWarning("RemoteErrorObject created. Error: %s", errorMessage.c_str());
}

void RemoteErrorObject::draw_inspector() {
	ImGui::Text("Missing RemoteObject");
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::Text("Error Message: %s", errorMessage.c_str());
}

void RemoteErrorObject::draw_hierarchy(Reference<const EditorSelectObject> select) {
	bool isSelected = select->is_selected(this);

	int flags =
		ImGuiTreeNodeFlags_DrawLinesToNodes |
		ImGuiTreeNodeFlags_FramePadding |
		ImGuiTreeNodeFlags_SpanAllColumns |
		ImGuiTreeNodeFlags_Leaf;
	if (isSelected) {
		flags |= ImGuiTreeNodeFlags_Selected; // 選択時は選択状態にする
	}
	ImGui::TreeNodeEx(std::format("Missing RemoteObject##{}", (void*)this).c_str(), flags);
	ImGui::TreePop();

	// こうすると選択できるらしい
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen() && !isSelected) {
		EditorCommandInvoker::Execute(
			std::make_unique<EditorSelectCommand>(this)
		);
	}
}

std::unique_ptr<IRemoteObject> RemoteErrorObject::move_force([[maybe_unused]] Reference<const IRemoteObject> child) {
	return nullptr;
}

void RemoteErrorObject::reparent(Reference<IRemoteObject> remoteObject) {
	parent = remoteObject;
}

void RemoteErrorObject::add_child(std::unique_ptr<IRemoteObject>) {
	szgWarning("RemoteErrorObject cannot have children.");
}

nlohmann::json RemoteErrorObject::serialize() const {
	nlohmann::json result;

	result["Type"] = instance_type();
	result.update(hierarchyName);

	return result;
}

#endif // DEBUG_FEATURES_ENABLE
