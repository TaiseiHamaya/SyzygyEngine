#include "EditorAssetImporter.h"

#include <array>

#include <imgui.h>

using namespace szg;

void EditorAssetImporter::add_package(const std::filesystem::path& filePath) {
	loadAssets.emplace_back(filePath, optimizer.is_support_optimization(filePath.extension()), false);
	isShowLoadPopup = true;
}

void szg::EditorAssetImporter::update() {
	show_load_popup();
}

std::optional<std::filesystem::path>& szg::EditorAssetImporter::import_filepath_mut() {
	return importFilePath;
}

void EditorAssetImporter::show_load_popup() {
	if (!isShowLoadPopup) {
		loadAssets.clear();
		return;
	}

	ImGui::Begin("AssetImporter", &isShowLoadPopup, ImGuiWindowFlags_NoDocking);

	if (!importFilePath.has_value()) {
		// 保存できない状態の場合、ボタンを無効化
		ImGui::BeginDisabled();
	}

	if (ImGui::Button("Import")) {
		execute_import();
		isShowLoadPopup = false;
	}
	if (!importFilePath.has_value()) {
		ImGui::EndDisabled();
	}

	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		loadAssets.clear();
		isShowLoadPopup = false;
	}

	if (!importFilePath.has_value()) {
		ImGui::SameLine();
		ImGui::TextColored(ImVec4{ 0.9f,0.1f,0.1f,1.0f }, "Please select import destination path.");
	}

	ImGui::Separator();

	i32 optimizeState = 0;
	bool isAllSelected = true;
	bool isAnySelected = false;
	for (const auto& loadInfo : loadAssets) {
		if (!loadInfo.isSupportOptimization) {
			continue;
		}
		isAllSelected &= loadInfo.useOptimization;
		isAnySelected |= loadInfo.useOptimization;
	}
	optimizeState = isAllSelected ? 0b11 : (isAnySelected ? 0b01 : 0b00);
	if (ImGui::CheckboxFlags("OptimizeAll", &optimizeState, 0b11)) {
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

	constexpr i32 columnCount = 2;
	i32 tableFlags =
		ImGuiTableFlags_Borders |
		ImGuiTableFlags_PreciseWidths |
		ImGuiTableFlags_SizingFixedFit |
		ImGuiTableFlags_NoHostExtendX |
		ImGuiTableFlags_RowBg;

	ImGui::BeginTable("PackageLoadTable", columnCount, tableFlags);
	constexpr std::array<string_literal, columnCount> HEADER_NAME = {
		"Optimize",
		"File",
	};
	ImGui::TableSetupColumn(HEADER_NAME[0], ImGuiTableColumnFlags_PreferSortDescending);
	ImGui::TableSetupColumn(HEADER_NAME[1]);
	ImGui::TableHeadersRow();

	// TableItem
	for (auto& loadInfo : loadAssets) {
		ImGui::TableNextRow();

		// 最適化チェックボックス
		ImGui::TableSetColumnIndex(0);
		if (loadInfo.isSupportOptimization) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::Checkbox(std::format("##{}", loadInfo.filePath.string()).c_str(), &loadInfo.useOptimization);
			ImGui::PopStyleVar();
		}
		else {
			ImGui::TextDisabled("N/A");
			loadInfo.useOptimization = true;
		}

		// ファイル名
		ImGui::TableSetColumnIndex(1);
		ImGui::Text(std::format("{}", loadInfo.filePath.filename().string()).c_str());
	}

	ImGui::EndTable();

	ImGui::End();
}

void szg::EditorAssetImporter::execute_import() {
	for (const auto& loadInfo : loadAssets) {
		if (loadInfo.useOptimization && loadInfo.isSupportOptimization) {
			optimizer.optimize_asset(loadInfo.filePath, importFilePath.value());
		}
		else {
			// 最適化しない場合はそのままコピー
			std::filesystem::copy_file(loadInfo.filePath, importFilePath.value() / loadInfo.filePath.filename(), std::filesystem::copy_options::overwrite_existing);
		}
	}

	loadAssets.clear();
}
