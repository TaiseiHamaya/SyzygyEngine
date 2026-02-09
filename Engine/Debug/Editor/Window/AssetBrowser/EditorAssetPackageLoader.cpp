#include "EditorAssetPackageLoader.h"

#include <imgui.h>

using namespace szg;

void EditorAssetPackageLoader::add_package(const std::filesystem::path& filePath) {
	loadAssets.emplace_back(filePath, false);
	isShowLoadPopup = true;
}

void szg::EditorAssetPackageLoader::update() {
	show_load_popup();
}

void EditorAssetPackageLoader::show_load_popup() {
	if (!isShowLoadPopup) {
		loadAssets.clear();
		return;
	}

	ImGui::Begin("Import package", &isShowLoadPopup, ImGuiWindowFlags_NoDocking);

	i32 optimizeState = 0;
	bool isAllSelected = true;
	bool isAnySelected = false;
	for (const auto& loadInfo : loadAssets) {
		isAllSelected &= loadInfo.useOptimization;
		isAnySelected |= loadInfo.useOptimization;
	}
	optimizeState = isAllSelected ? 0b11 : (isAnySelected ? 0b01 : 0b00);
	ImGui::BeginTable("PackageLoadTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
	ImGui::TableSetupColumn("File", ImGuiTableColumnFlags_WidthStretch);
	ImGui::TableSetupColumn("Optimize", ImGuiTableColumnFlags_WidthFixed, 100.0f);

	// header
	string_literal column_name = "Optimize";
	ImGui::PushID(1);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	if (ImGui::CheckboxFlags("Optimize##All", &optimizeState, 0b11)) {
		if (isAllSelected) {
			// 全選択解除
			for (auto& loadInfo : loadAssets) {
				loadInfo.useOptimization = false;
			}
		}
		else {
			// 全選択
			for (auto& loadInfo : loadAssets) {
				loadInfo.useOptimization = true;
			}
		}
	}
	ImGui::PopStyleVar();
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::TableHeader(column_name);
	ImGui::PopID();

	// TableItem
	for (auto& loadInfo : loadAssets) {
		ImGui::TableNextRow();

		// ファイル名
		ImGui::TableSetColumnIndex(0);
		ImGui::Text(std::format("{}", loadInfo.filePath.filename().string()).c_str());

		// 最適化チェックボックス
		ImGui::TableSetColumnIndex(1);
		ImGui::CheckboxFlags(std::format("Optimize##{}", loadInfo.filePath.string()).c_str(), reinterpret_cast<i32*>(&loadInfo.useOptimization), 0b11);
	}

	ImGui::EndTable();

	if (ImGui::Button("Import")) {
		execute_load();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		loadAssets.clear();
		isShowLoadPopup = false;
	}

	ImGui::End();
}

void EditorAssetPackageLoader::execute_load() {
	loadAssets.clear();
}
