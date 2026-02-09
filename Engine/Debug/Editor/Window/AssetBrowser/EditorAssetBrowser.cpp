#include "EditorAssetBrowser.h"

using namespace szg;

#include <imgui.h>

#include "Engine/Application/Logger.h"

void EditorAssetBrowser::draw() {
	ImGui::Begin("Asset", &isActive);
	update_focus();

	// ヘッダの描画
	draw_header();

	ImGui::Separator();

	// ファイルの描画
	draw_assets();

	ImGui::Separator();

	draw_footer();

	// 右クリックメニュー
	draw_right_click_menu();

	// Drag＆Drop関連
	handle_drag_and_drop();

	ImGui::End();

	// パッケージ読み込みポップアップ
	update_importer();
}

void szg::EditorAssetBrowser::on_drop_file(const std::filesystem::path& filePath) {
	assetImporter.add_package(filePath);
}

void szg::EditorAssetBrowser::draw_header() {
	if (ImGui::Button("\ue5d8")) {
		// ディレクトリを1つ上に移動
		if (currentDirectory.empty()) {
			// 未選択状態にする
			rootType = AssetRootType::UNSELECT;
		}
		else {
			currentDirectory = currentDirectory.parent_path();
		}
		selectFileName.clear();
	}
	// 現在のディレクトリを表示
	if (rootType == AssetRootType::UNSELECT) {
		ImGui::Text(
			std::format("{}->{}", ROOT_TAG[static_cast<i32>(AssetRootType::UNSELECT)], selectFileName).c_str()
		);
	}
	else {
		ImGui::Text(
			(ROOT_TAG[static_cast<i32>(rootType)] / currentDirectory / selectFileName)
			.string().c_str()
		);
	}
}

void szg::EditorAssetBrowser::draw_assets() {
	lastCursorPos = {
		ImGui::GetCursorPosX(), ImGui::GetCursorPosY()
	};
	nextLinePosY = 0.0f;

	ImVec2 childSize{ 0.0f, 0.0f };
	childSize.y = ImGui::GetContentRegionAvail().y - 20;
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.01f,0.01f,0.01f,1.0f });
	ImGui::BeginChild("AssetBrowserFolderChild", childSize, ImGuiChildFlags_FrameStyle);
	ImGui::PopStyleColor();

	// ルートが選択されていない場合は選択肢を表示
	if (rootType == AssetRootType::UNSELECT) {
		draw_file_content(ROOT_TAG[static_cast<i32>(AssetRootType::ENGINE)], true, 0);
		draw_file_content(ROOT_TAG[static_cast<i32>(AssetRootType::GAME)], true, 1);

		if (currentDirectory == ROOT_TAG[static_cast<i32>(AssetRootType::ENGINE)]) {
			rootType = AssetRootType::ENGINE;
			selectFileName.clear();
			currentDirectory.clear();
		}
		if (currentDirectory == ROOT_TAG[static_cast<i32>(AssetRootType::GAME)]) {
			rootType = AssetRootType::GAME;
			selectFileName.clear();
			currentDirectory.clear();
		}
	}
	else {
		auto directory = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory;

		// ディレクトリ内のファイル一覧を取得してソート
		std::vector<std::filesystem::directory_entry> entries;
		for (auto& entry : std::filesystem::directory_iterator{ directory }) {
			entries.emplace_back(entry);
		}

		std::ranges::sort(entries, [](const auto& l, const auto& r) {
			// フォルダ→ファイルの順にソート
			if (l.is_directory() != r.is_directory()) {
				return l.is_directory() > r.is_directory();
			}
			// 同じ種類の場合は名前順にソート
			return l.path().filename() < r.path().filename();
		});

		// 描画
		for (i32 i = 0; auto file : entries) {
			std::string filename = file.path().filename().string();
			draw_file_content(filename, file.is_directory(), i);
			++i;
		}
	}

	if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		selectFileName.clear();
	}

	ImGui::EndChild();
}

void szg::EditorAssetBrowser::draw_footer() {
	// アイコンサイズの調整スライダー
	ImGui::Text("Icon Size:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(150.0f);
	i32 flags =
		ImGuiSliderFlags_AlwaysClamp |
		ImGuiSliderFlags_NoInput;
	ImGui::SliderFloat("##IconSize", &iconSize, 24.0f, 256.0f, "", flags);
}

void szg::EditorAssetBrowser::draw_right_click_menu() {
	// 右クリックメニュー
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
		ImGui::OpenPopup("AssetBrowserRightClickMenu");
	}

	if (ImGui::BeginPopup("AssetBrowserRightClickMenu")) {
		std::filesystem::path directory = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory / selectFileName;


		ImGui::EndPopup();
	}
}

void szg::EditorAssetBrowser::handle_drag_and_drop() {
}

void szg::EditorAssetBrowser::draw_file_content(const std::string& name, bool isDirectory, i32 idx) {
	// ファイル一覧の表示
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 padding = style.ItemSpacing;

	// 1行の個数を計算
	i32 numPerRow = static_cast<i32>(ImGui::GetContentRegionAvail().x / (iconSize + padding.x));
	if (numPerRow < 1) {
		numPerRow = 1;
	}

	// フォルダ or ファイルアイコン
	string_literal icon = isDirectory ? "\ue2c7" : "\ue66d";
	ImVec4 color = isDirectory ? ImVec4{ 0.8f, 0.8f, 0.2f, 1.0f } : ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

	bool isSelected = (selectFileName == name);

	if (idx % numPerRow != 0) {
		// 同じ行の場合は同じ行に表示
		ImGui::SetCursorPos(
			ImVec2{
				lastCursorPos.x + (iconSize + padding.x),
				lastCursorPos.y
			}
		);
	}
	else {
		ImGui::SetCursorPos(
			ImVec2{
				padding.x,
				nextLinePosY
			}
		);
		nextLinePosY = 0;
	}

	lastCursorPos = {
		ImGui::GetCursorPosX(),
		ImGui::GetCursorPosY()
	};

	ImGui::SetNextItemAllowOverlap();
	i32 flags = ImGuiSelectableFlags_AllowDoubleClick;
	ImGui::PushFont(ImGui::GetFont(), iconSize);
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::Selectable(std::format("{}##{}", icon, name).c_str(), &isSelected, flags, ImVec2{ iconSize, iconSize });
	ImGui::PopStyleColor();
	ImGui::PopFont();

	// シングルクリックで選択
	if (isSelected) {
		selectFileName = name;
	}

	// ダブルクリックでフォルダ移動またはファイルオープン
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
		if (isDirectory) {
			// フォルダの場合は移動
			currentDirectory /= name;
			selectFileName.clear();
		}
		else {
			// ファイルの場合は開く
			szgInformation("Open Asset File: {}", name);
		}
	}

	// ファイル名表示
	ImGui::SetCursorPos(
		ImVec2{
			lastCursorPos.x,
			lastCursorPos.y + iconSize + padding.y
		}
	);
	ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + iconSize);
	i32 maxLabelLength = static_cast<i32>(iconSize / 2.6f);
	std::string label = name.size() > maxLabelLength ? name.substr(0, maxLabelLength - 3) + "..." : name;
	ImGui::Text(label.c_str());

	nextLinePosY = std::max(nextLinePosY, ImGui::GetCursorPosY() + padding.y);
}

void szg::EditorAssetBrowser::update_importer() {
	// 保存先ディレクトリの設定
	auto directory = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory / selectFileName;

	if (std::filesystem::is_directory(directory) && rootType == AssetRootType::GAME) {
		// フォルダの場合ディレクトリを更新
		assetImporter.import_filepath_mut() = directory;
	}
	else {
		// ファイルの場合、保存できない状態にする
		assetImporter.import_filepath_mut().reset();
	}

	// 更新
	assetImporter.update();
}
