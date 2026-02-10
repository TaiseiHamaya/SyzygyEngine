#ifdef DEBUG_FEATURES_ENABLE

#include "FolderObject.h"

using namespace szg;

#include <format>

#include <imgui.h>

#include "../Command/EditorCommandInvoker.h"
#include "../Command/EditorSelectCommand.h"
#include "../Core/EditorDandDManager.h"

void FolderObject::setup() {
	for (auto& child : children) {
		child->setup();
	}
}

void FolderObject::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	for (auto& child : children) {
		child->update_preview(world, parentAffine);
	}
}

void FolderObject::draw_inspector() {
	hierarchyName.show_gui();
}

void FolderObject::draw_hierarchy(Reference<const EditorSelectObject> select) {
	bool isSelected = select->is_selected(this);

	int flags =
		ImGuiTreeNodeFlags_DrawLinesToNodes |
		ImGuiTreeNodeFlags_FramePadding |
		ImGuiTreeNodeFlags_SpanAllColumns |
		ImGuiTreeNodeFlags_NoTreePushOnOpen |
		ImGuiTreeNodeFlags_OpenOnArrow; // 矢印で開く
	if (isSelected) {
		flags |= ImGuiTreeNodeFlags_Selected; // 選択時は選択状態にする
	}
	if (isOpen) {
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}
	isOpen = ImGui::TreeNodeEx(std::format("{}##{}", hierarchyName.value_mut(), (void*)this).c_str(), flags);
	EditorDandDManager::CheckDandDHierarchy(this, parent);

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen() && !isSelected) {
		EditorCommandInvoker::Execute(
			std::make_unique<EditorSelectCommand>(this)
		);
	}

	if (isOpen) {
		for (auto& child : children) {
			if (!child) {
				continue;
			}
			child->draw_hierarchy(select);
		}
		ImGui::Separator();
	}
}

std::unique_ptr<IRemoteObject> FolderObject::move_force(Reference<const IRemoteObject> child) {
	auto itr = std::find_if(children.begin(), children.end(),
	[&](const std::unique_ptr<IRemoteObject>& lhs) {
		return lhs.get() == child.ptr();
	});
	if (itr != children.end()) {
		std::unique_ptr<IRemoteObject> childObject = std::move(*itr);
		children.erase(itr);
		return childObject;
	}
	return nullptr;
}

void FolderObject::reparent(Reference<IRemoteObject> remoteObject) {
	parent = remoteObject;
}

void FolderObject::add_child(std::unique_ptr<IRemoteObject> child) {
	child->reparent(this);
	children.emplace_back(std::move(child));
}

nlohmann::json FolderObject::serialize() const {
	nlohmann::json result;

	result["Type"] = instance_type();
	result.update(hierarchyName);
	result["Children"] = nlohmann::json::array();
	for (const auto& child : children) {
		result["Children"].emplace_back(child->serialize());
	}

	return result;
}

void FolderObject::on_spawn() {
	for (auto& child : children) {
		if (child) {
			child->on_spawn();
		}
	}
}

void FolderObject::on_destroy() {
	for (auto& child : children) {
		if (child) {
			child->on_destroy();
		}
	}
}

#endif // DEBUG_FEATURES_ENABLE
