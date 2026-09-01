#include "RemoteColliderGroupRegistry.h"

#include <format>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "Engine/Assets/Json/JsonAsset.h"
#include "Engine/Debug/Editor/Command/EditorCommandResizeContainer.h"
#include "Engine/Debug/Editor/Command/EditorCommandScope.h"
#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"

using namespace szg;

void szg::RemoteColliderGroupRegistry::load(const std::string& sceneName) {
	JsonAsset json{ std::format("./Game/Core/Scene/{}/CollisionGroup.json", sceneName) };

	if (json.cget().is_null()) {
		return;
	}
}

void szg::RemoteColliderGroupRegistry::show_gui() {
	int flags =
		ImGuiTreeNodeFlags_FramePadding |
		ImGuiTreeNodeFlags_SpanLabelWidth |
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

			if (renameGroupName.has_value() && isSelected) {
				std::string& prevGroupName = renameGroupName.value();
				ImGui::SetNextItemWidth(100);
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
				ImGui::Selectable(std::format("{}##{}", entry.name, id).c_str(), isSelected, 0, ImVec2{ 100, 0 });

				ImGui::SameLine();

				std::string guiId = std::format("{}{:#010x}", entry.name, id);
				if (ImGui::Button(std::format("\ue676##{}", guiId).c_str())) {
					renameGroupName = entry.name;
					selecteIdx = id;
				}

				ImGui::SameLine();
				if (ImGui::Button(std::format("\ue15b##{}", guiId).c_str())) {
					EditorValueChangeCommandHandler::GenCommandInstant<bool>(colliderGroups, id, &ColliderGroupEntry::isDeleted, true);
				}
			}
		}

		if (ImGui::Button("\ue145")) {
			EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());

			i32 size = static_cast<i32>(colliderGroups.size()) + 1;
			i32 idx = size - 1;

			EditorCommandInvoker::Execute(std::make_unique<EditorCommandResizeContainer<decltype(groupEditorGuiOrder)>>(groupEditorGuiOrder, size));
			EditorCommandInvoker::Execute(std::make_unique<EditorCommandResizeContainer<decltype(colliderGroups)>>(colliderGroups, size));

			EditorValueChangeCommandHandler::GenCommandInstant<std::string>(colliderGroups, idx, &ColliderGroupEntry::name, std::format("New Group {}", idx));
			EditorValueChangeCommandHandler::GenCommandInstant<i32>(groupEditorGuiOrder, idx, std::identity{}, idx);

			EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
		}

		ImGui::TreePop();
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
