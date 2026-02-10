#ifdef DEBUG_FEATURES_ENABLE

#include "EditorDandDManager.h"

using namespace szg;

#include <imgui.h>

#include "Engine/Application/Logger.h"
#include "Engine/Debug/Editor/Command/EditorCommandInvoker.h"
#include "Engine/Debug/Editor/Command/EditorCommandReparent.h"

void EditorDandDManager::CheckDandDHierarchy(Reference<IRemoteObject> self, Reference<IRemoteObject> parent) {
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		BeginDrag(self, parent);
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget()) {
		EndDrag(self);
		ImGui::EndDragDropTarget();
	}
}

void EditorDandDManager::BeginDrag(Reference<IRemoteObject> self, Reference<IRemoteObject> parent) {
	if (!parent || !self) {
		return;
	}
	auto& instance = GetInstance();
	instance.dragData.parent = parent;
	instance.dragData.dragging = self;
	if (ImGui::GetDragDropPayload() == nullptr) {
		ImGui::SetDragDropPayload("EditorDandDManager", &instance.dragData, sizeof(DragDataHierarchy), ImGuiCond_Once);
		szgInformation("Begin drag&drop.");
	}
}

void EditorDandDManager::EndDrag(Reference<IRemoteObject> target) {
	auto& instance = GetInstance();
	if (!target) {
		szgError("Get ImGui drag&drop payload but target is nullptr.");
		return;
	}
	const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EditorDandDManager");
	if (!payload) {
		return;
	}
	if (payload->DataSize != sizeof(DragDataHierarchy)) {
		szgError("Get ImGui drag&drop payload but DragDataHierarchy size is mismatch.");
		return;
	}

	if (instance.dragData.parent == target) {
		// 同じ位置にドロップした場合は何もしない
		szgInformation("Drop to same parent.");
		return;
	}

	Reference<IRemoteObject> targetParent = target->get_parent();
	while (targetParent) {
		if (targetParent == instance.dragData.dragging) {
			// ドロップ先がターゲットの子孫である場合は何もしない
			szgWarning("Can't drop to target parent.");
			return;
		}
		targetParent = targetParent->get_parent();
	}

	instance.command =
		std::make_unique<EditorCommandReparent>(instance.dragData.dragging, instance.dragData.parent, target);
	szgInformation("Successed drag&drop.");
}

void EditorDandDManager::ExecuteReparent() {
	auto& instance = GetInstance();
	if (!instance.command) {
		return;
	}
	EditorCommandInvoker::Execute(
		std::move(instance.command)
	);
}

#endif // DEBUG_FEATURES_ENABLE
