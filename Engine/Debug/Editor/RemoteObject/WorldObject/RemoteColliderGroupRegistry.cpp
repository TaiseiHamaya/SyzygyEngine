#include "RemoteColliderGroupRegistry.h"

#include <format>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "Engine/Debug/Editor/Command/EditorCommandResizeContainer.h"
#include "Engine/Debug/Editor/Command/EditorCommandScope.h"
#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"

void szg::RemoteColliderGroupRegistry::load(const std::string& sceneName) {
	sceneName;
}

void szg::RemoteColliderGroupRegistry::show_gui() {
	int flags =
		ImGuiTreeNodeFlags_FramePadding |
		ImGuiTreeNodeFlags_SpanAllColumns |
		ImGuiTreeNodeFlags_OpenOnArrow; // 矢印で開く
	if (isOpen) {
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}
	isOpen = ImGui::TreeNodeEx("Collider Groups", flags);
	if (isOpen) {
		for (auto id : groupEditorGuiOrder) {
			auto& entry = colliderGroups.at(id);

			if (entry.isDeleted) {
				continue;
			}

			bool isSelected = selecteIdx.value_or(-1) == id;

			if (renameGroupName.has_value()) {
				std::string& prevGroupName = renameGroupName.value();
				ImGui::InputText("##Rename", &entry.name);
				if (ImGui::IsItemDeactivated()) {
					if (!entry.name.empty() && entry.name != prevGroupName) {
						std::swap(entry.name, prevGroupName);
						EditorValueChangeCommandHandler::GenCommandInstant<std::string>(colliderGroups, id, &ColliderGroupEntry::name, prevGroupName);
					}

					renameGroupName.reset();
				}
			}
			else {
				ImGui::Selectable(entry.name.c_str(), isSelected);

				ImGui::SameLine();

				if (ImGui::Button(std::format("\ue676##{}", entry.name).c_str())) {
					renameGroupName = entry.name;
				}

				if (ImGui::Button(std::format("\ue15b##{}", entry.name).c_str())) {
					EditorValueChangeCommandHandler::GenCommandInstant<bool>(colliderGroups, id, &ColliderGroupEntry::isDeleted, true);
				}
			}
		}

		ImGui::TreePop();
	}

	if (ImGui::Button("\ue145")) {
		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());

		i32 idx = static_cast<i32>(colliderGroups.size()) + 1;

		EditorCommandInvoker::Execute(std::make_unique<EditorCommandResizeContainer<decltype(groupEditorGuiOrder)>>(groupEditorGuiOrder, idx));
		EditorCommandInvoker::Execute(std::make_unique<EditorCommandResizeContainer<decltype(colliderGroups)>>(colliderGroups, idx));

		EditorValueChangeCommandHandler::GenCommandInstant<std::string>(colliderGroups, idx, &ColliderGroupEntry::name, "New Group");
		EditorValueChangeCommandHandler::GenCommandInstant<i32>(groupEditorGuiOrder, idx, std::identity{}, idx);

		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
	}
}

std::optional<i32> szg::RemoteColliderGroupRegistry::combo_gui(const i32 current, const std::string& label) const {
	std::optional<i32> result;
	auto& currentGroup = colliderGroups.at(current);

	if (ImGui::BeginCombo(label.c_str(), currentGroup.isDeleted ? "Unknown" : currentGroup.name.c_str())) {
		for (auto id : groupEditorGuiOrder) {
			auto& entry = colliderGroups.at(id);
			if (entry.isDeleted) {
				continue;
			}

			bool isSelected = (current == id);
			if (ImGui::Selectable(entry.name.c_str(), isSelected)) {
				result = id;
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	return result;
}
