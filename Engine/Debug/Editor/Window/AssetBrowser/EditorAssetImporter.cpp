#include "EditorAssetImporter.h"

#include <array>

#include <imgui.h>

using namespace szg;

void EditorAssetImporter::add_package(const std::filesystem::path& filePath) {
	loadAssets.emplace_back(filePath, optimizer.is_support_optimization(filePath.extension()), false);
	isShowLoadPopup = true; // ポップアップ表示フラグを立てる
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

	// ヘッダー
	// インポートボタン
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
	// キャンセルボタン
	if (ImGui::Button("Cancel")) {
		loadAssets.clear();
		isShowLoadPopup = false;
	}

	if (!importFilePath.has_value()) {
		ImGui::SameLine();
		ImGui::TextColored(ImVec4{ 0.9f,0.1f,0.1f,1.0f }, "Please select import destination path.");
	}

	ImGui::Separator();

	// optimize all チェックボックス
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
			// 全選択されている場合は全選択解除
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

	// Importするファイルの一覧
	constexpr i32 columnCount = 2;
	i32 tableFlags =
		ImGuiTableFlags_Borders | // 外枠と内枠の表示
		ImGuiTableFlags_PreciseWidths | // 列幅を正確に指定
		ImGuiTableFlags_SizingFixedFit | // 列幅の自動調整
		ImGuiTableFlags_NoHostExtendX | // テーブルの幅をウィンドウ幅に合わせない
		ImGuiTableFlags_RowBg; // 行の背景色を交互に変更

	ImGui::BeginTable("PackageLoadTable", columnCount, tableFlags);
	constexpr std::array<string_literal, columnCount> HEADER_NAME = {
		"Optimize",
		"File",
	};
	ImGui::TableSetupColumn(HEADER_NAME[0], ImGuiTableColumnFlags_PreferSortDescending);
	ImGui::TableSetupColumn(HEADER_NAME[1]);
	ImGui::TableHeadersRow(); // ヘッダー行の表示

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
			loadInfo.useOptimization = false;
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
			std::filesystem::copy(
				loadInfo.filePath,
				importFilePath.value() / loadInfo.filePath.filename(),
				std::filesystem::copy_options::overwrite_existing
			);
		}
	}

	loadAssets.clear();
}
