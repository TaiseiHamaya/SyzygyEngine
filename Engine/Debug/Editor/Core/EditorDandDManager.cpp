#ifdef DEBUG_FEATURES_ENABLE

#include "EditorDandDManager.h"

using namespace szg;

#include <imgui.h>

#include "Engine/Application/Logger.h"
#include "Engine/Debug/Editor/Command/EditorCommandInvoker.h"
#include "Engine/Debug/Editor/Command/EditorCommandReparent.h"

void EditorDandDManager::CheckDandDHierarchy(Reference<IRemoteObject> self, Reference<IRemoteObject> parent) {
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		BeginDragHierarchy(self, parent);
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget()) {
		EndDragHierarchy(self);
		ImGui::EndDragDropTarget();
	}
}

void EditorDandDManager::BeginDragHierarchy(Reference<IRemoteObject> self, Reference<IRemoteObject> parent) {
	if (!parent || !self) {
		return;
	}
	auto& instance = GetInstance();
	DragDataHierarchy data{
		.parent = parent,
		.dragging = self
	};
	instance.currentDragData = data;
	if (ImGui::GetDragDropPayload() == nullptr) {
		ImGui::SetDragDropPayload("EditorDandDManager", &instance.currentDragData, sizeof(instance.currentDragData), ImGuiCond_Once);
		szgInformation("Begin drag&drop as hierarchy.");
	}
}

void EditorDandDManager::EndDragHierarchy(Reference<IRemoteObject> target) {
	auto& instance = GetInstance();
	if (!target) {
		szgError("Get ImGui drag&drop payload but target is nullptr.");
		return;
	}
	const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EditorDandDManager");
	if (!payload) {
		return;
	}
	if (payload->DataSize != sizeof(instance.currentDragData)) {
		szgError("Get ImGui drag&drop payload but DragDataHierarchy size is mismatch.");
		return;
	}

	if (!std::holds_alternative<DragDataHierarchy>(instance.currentDragData)) {
		szgError("Get ImGui drag&drop payload but currentDragData is not DragDataHierarchy.");
		return;
	}

	DragDataHierarchy& data = std::get<DragDataHierarchy>(instance.currentDragData);

	if (data.parent == target) {
		// 同じ位置にドロップした場合は何もしない
		szgInformation("Drop to same parent.");
		return;
	}

	Reference<IRemoteObject> targetParent = target->get_parent();
	while (targetParent) {
		if (targetParent == data.dragging) {
			// ドロップ先がターゲットの子孫である場合は何もしない
			szgWarning("Can't drop to target parent.");
			return;
		}
		targetParent = targetParent->get_parent();
	}

	instance.command =
		std::make_unique<EditorCommandReparent>(data.dragging, data.parent, target);
	szgInformation("Succussed drag&drop.");

	instance.currentDragData = std::monostate{};
}

void szg::EditorDandDManager::BeginDragAsset(AssetType assetType, const std::string& filePath) {
	auto& instance = GetInstance();
	DragDataAsset data{
		.assetType = assetType,
		.filePath = filePath
	};
	instance.currentDragData = data;
	if (ImGui::GetDragDropPayload() == nullptr) {
		ImGui::SetDragDropPayload("EditorDandDManager", &instance.currentDragData, sizeof(instance.currentDragData), ImGuiCond_Once);
		szgInformation("Begin drag&drop as asset.");
	}
}

std::optional<EditorDandDManager::DragDataAsset> EditorDandDManager::AcceptAssetDrop() {
	if (!ImGui::BeginDragDropTarget()) {
		return std::nullopt;
	}
	const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EditorDandDManager");
	if (!payload) {
		ImGui::EndDragDropTarget();
		return std::nullopt;
	}
	auto& instance = GetInstance();
	if (!std::holds_alternative<DragDataAsset>(instance.currentDragData)) {
		ImGui::EndDragDropTarget();
		return std::nullopt;
	}
	DragDataAsset result = std::get<DragDataAsset>(instance.currentDragData);
	instance.currentDragData = std::monostate{};
	ImGui::EndDragDropTarget();
	return result;
}

void EditorDandDManager::ExecuteCommand() {
	auto& instance = GetInstance();
	if (!instance.command) {
		return;
	}
	EditorCommandInvoker::Execute(
		std::move(instance.command)
	);
}

#endif // DEBUG_FEATURES_ENABLE
