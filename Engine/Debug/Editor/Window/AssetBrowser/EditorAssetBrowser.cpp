#include "EditorAssetBrowser.h"

using namespace szg;

#include <imgui.h>
#include <imgui_stdlib.h>
#include <Windows.h>

#include "Engine/Application/Logger.h"

#include "Engine/Debug/Editor/Core/EditorDandDManager.h"
#include "Engine/Debug/Editor/Core/EditorAssetContentsCollector.h"

void EditorAssetBrowser::draw() {
	ImGui::Begin("Asset", &isActive);
	update_focus();

	// ヘッダの描画
	draw_header();

	ImGui::Separator();

	// ファイルの描画
	draw_assets();

	ImGui::Separator();

	// フッターの描画
	draw_footer();

	// 右クリックメニュー
	draw_right_click_menu();

	// Drag＆Drop関連
	handle_drag_and_drop();

	// ショートカットキーの処理
	update_shortcut();

	ImGui::End();

	// パッケージ読み込みポップアップ
	update_importer();
}

void szg::EditorAssetBrowser::on_drop_file(const std::filesystem::path& filePath) {
	assetImporter.add_package(filePath);
}

void szg::EditorAssetBrowser::draw_header() {
	// 一つ上のディレクトリに移動ボタン
	if (ImGui::Button("\ue5d8")) {
		if (currentDirectory.empty()) {
			// ルートから更に上に移動しようとした場合はルート未選択状態に戻す
			rootType = AssetRootType::Unselect;
		}
		else {
			// ディレクトリを1つ上に移動
			currentDirectory = currentDirectory.parent_path();
		}
		// 選択ファイルの解除
		selectFileName.clear();
	}

	// 現在のディレクトリを表示
	if (rootType == AssetRootType::Unselect) {
		ImGui::Text(
			std::format("{}->{}", ROOT_TAG[static_cast<i32>(AssetRootType::Unselect)], selectFileName).c_str()
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
	childSize.y = ImGui::GetContentRegionAvail().y - 28; // フッター分を引く
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.01f,0.01f,0.01f,1.0f });
	ImGui::BeginChild("AssetBrowserFolderChild", childSize, ImGuiChildFlags_FrameStyle);
	ImGui::PopStyleColor();

	// ルートが選択されていない場合は選択肢を表示
	if (rootType == AssetRootType::Unselect) {
		// エンジン、ゲームのフォルダを表示
		draw_file_content(ROOT_TAG[static_cast<i32>(AssetRootType::Engine)], true, 0);
		draw_file_content(ROOT_TAG[static_cast<i32>(AssetRootType::Game)], true, 1);

		// ルートフォルダが選択された場合の処理
		if (currentDirectory == ROOT_TAG[static_cast<i32>(AssetRootType::Engine)]) {
			rootType = AssetRootType::Engine;
			selectFileName.clear();
			currentDirectory.clear();
		}
		if (currentDirectory == ROOT_TAG[static_cast<i32>(AssetRootType::Game)]) {
			rootType = AssetRootType::Game;
			selectFileName.clear();
			currentDirectory.clear();
		}
	}
	else {
		auto directory = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory;

		// 存在しないディレクトリの場合は親ディレクトリに移動
		while (!std::filesystem::exists(directory)) {
			// Rootフォルダまで来ても存在しない場合は作成
			if (currentDirectory.empty()) {
				std::error_code ec;
				std::filesystem::create_directories(ROOT_PATH[static_cast<i32>(rootType)], ec);
				if (ec) {
					szgError("Failed to create root asset directory: {}", ec.message());
				}
				directory = ROOT_PATH[static_cast<i32>(rootType)];
				break;
			}
			currentDirectory = currentDirectory.parent_path();
			directory = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory;
		}

		// ディレクトリ内のファイル一覧を取得してソート
		std::vector<std::filesystem::directory_entry> entries;
		for (auto& entry : std::filesystem::directory_iterator{ directory }) {
			entries.emplace_back(entry);
		}

		std::ranges::sort(entries, [](const auto& l, const auto& r) {
			// フォルダ→ファイルの順になるようソート
			if (l.is_directory() != r.is_directory()) {
				return l.is_directory() > r.is_directory();
			}
			// 同じ種類の場合は名前順にソート
			return l.path().filename() < r.path().filename();
		});

		// 描画
		for (i32 i = 0; auto file : entries) {
			draw_file_content(file.path().filename(), file.is_directory(), i);
			++i;
		}
	}

	// 何もないところをクリックした場合は選択解除
	if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		selectFileName.clear();
		if (isRenaming) {
			isRenaming = false;
			newFileName.clear();
		}
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
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
		ImGui::OpenPopup("AssetBrowserRightClickMenu");
	}

	if (ImGui::BeginPopup("AssetBrowserRightClickMenu")) {
		// エクスプローラーで開く
		std::filesystem::path directory = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory / selectFileName;
		if (ImGui::MenuItem("Open in Explorer")) {
			ShellExecuteW(NULL, L"open", directory.native().c_str(), NULL, NULL, SW_SHOWDEFAULT);
		}

		if (selectFileName.empty() && rootType == AssetRootType::Game) {
			// フォルダーの作成
			if (ImGui::MenuItem("Create Folder")) {
				std::filesystem::path newFolderPath = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory / "NewFolder";
				i32 count = 1;
				while (std::filesystem::exists(newFolderPath)) {
					newFolderPath = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory / std::format("NewFolder({})", count);
					++count;
				}
				std::error_code ec;
				std::filesystem::create_directory(newFolderPath, ec);
				if (ec) {
					szgError("Failed to create folder: {}", ec.message());
				}
				else {
					szgInformation("Created folder: {}", newFolderPath.string());
					selectFileName = newFolderPath.filename().string();
					newFileName = selectFileName;
					isRenaming = true;
				}
			}
		}

		if (!selectFileName.empty() && rootType == AssetRootType::Game) {
			// ファイル・フォルダの削除
			if (ImGui::MenuItem("Delete")) {
				std::error_code ec;
				std::filesystem::remove_all(directory, ec);
				if (ec) {
					szgError("Failed to delete file or directory: {}", ec.message());
				}
				else {
					szgInformation("Deleted file or directory: {}", directory.string());
					selectFileName.clear();
				}
			}

			// リネーム
			if (ImGui::MenuItem("Rename")) {
				isRenaming = true;
				newFileName = selectFileName;
			}
		}

		ImGui::EndPopup();
	}
}

void szg::EditorAssetBrowser::handle_drag_and_drop() {
}

void szg::EditorAssetBrowser::draw_file_content(const std::filesystem::path& fileName, bool isDirectory, i32 idx) {
	// ファイル一覧の表示
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 padding = style.ItemSpacing;
	const std::string fileNameString = fileName.string();

	// 1行の個数を計算
	i32 numPerRow = static_cast<i32>(ImGui::GetContentRegionAvail().x / (iconSize + padding.x));
	if (numPerRow < 1) {
		numPerRow = 1;
	}

	// フォルダ or ファイルアイコン
	string_literal icon = isDirectory ? "\ue2c7" : "\ue66d";
	ImVec4 color = isDirectory ? ImVec4{ 0.8f, 0.8f, 0.2f, 1.0f } : ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

	bool isSelected = (selectFileName == fileNameString);

	if (idx % numPerRow != 0) {
		// 同じ行の場合は前回の位置+アイコンサイズ分移動
		ImGui::SetCursorPos(
			ImVec2{
				lastCursorPos.x + (iconSize + padding.x),
				lastCursorPos.y
			}
		);
	}
	else {
		// 改行
		ImGui::SetCursorPos(
			ImVec2{
				padding.x,
				nextLinePosY // 前の行の最大Y位置に移動
			}
		);
		nextLinePosY = 0; // リセット
	}

	// 現在のカーソル位置を保存
	lastCursorPos = {
		ImGui::GetCursorPosX(),
		ImGui::GetCursorPosY()
	};

	// アイコン表示
	ImGui::SetNextItemAllowOverlap();
	i32 flags = ImGuiSelectableFlags_AllowDoubleClick;
	ImGui::PushFont(ImGui::GetFont(), iconSize);
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::Selectable(std::format("{}##{}", icon, fileNameString).c_str(), &isSelected, flags, ImVec2{ iconSize, iconSize });
	ImGui::PopStyleColor();
	ImGui::PopFont();

	// Drag source
	if (rootType != AssetRootType::Unselect) {
		bool canDrag = !isDirectory || rootType == AssetRootType::Game;
		if (canDrag && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
			AssetType assetType = isDirectory ? AssetType::Unknown : EditorAssetContentsCollector::GetAssetTypeByExtension(fileName.extension().string());
			EditorDandDManager::BeginDragAsset(assetType, fileNameString);
			ImGui::Text(fileNameString.c_str());
			ImGui::EndDragDropSource();
		}
	}

	// Drop target（ディレクトリへの移動）
	if (isDirectory && rootType == AssetRootType::Game) {
		if (auto dropData = EditorDandDManager::AcceptAssetDrop()) {
			std::filesystem::path srcPath = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory / dropData->filePath;
			std::filesystem::path dstPath = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory / fileNameString / dropData->filePath;
			std::error_code ec;
			std::filesystem::rename(srcPath, dstPath, ec);
			if (ec) {
				szgError("Failed to move: {}", ec.message());
			}
			else {
				szgInformation("Moved: {} -> {}", srcPath.string(), dstPath.string());
				if (selectFileName == dropData->filePath) {
					selectFileName.clear();
				}
			}
		}
	}

	// シングルクリックで選択
	if (isSelected) {
		selectFileName = fileNameString;
	}

	// ダブルクリックでフォルダ移動またはファイルオープン
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
		if (isDirectory) {
			// フォルダの場合は移動
			currentDirectory /= fileName;
			selectFileName.clear();
		}
		else {
			// ファイルの場合は開く
			szgInformation("Open Asset File: {}", fileNameString);
		}
	}

	// ファイル名表示
	ImGui::SetCursorPos( // アイコンの下に移動
		ImVec2{
			lastCursorPos.x,
			lastCursorPos.y + iconSize + padding.y
		}
	);
	if (isRenaming && isSelected) {
		// リネーム中
		ImGui::SetKeyboardFocusHere(); // フォーカスを当てる
		ImGui::SetNextItemWidth(iconSize); // アイコン幅に合わせる
		ImGui::InputText(
			std::format("##Rename{}", selectFileName).c_str(),
			&newFileName,
			ImGuiInputTextFlags_AutoSelectAll |
			ImGuiInputTextFlags_EnterReturnsTrue |
			ImGuiInputTextFlags_EscapeClearsAll
		);

		if (ImGui::IsItemDeactivated()) { // フォーカスが外れた場合も確定扱い
			if (!newFileName.empty() && selectFileName != newFileName) {
				// リネーム処理
				std::filesystem::path oldPath = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory / fileName;
				std::filesystem::path newPath = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory / newFileName;
				std::error_code ec;
				std::filesystem::rename(oldPath, newPath, ec);
				if (ec) {
					szgError("Failed to rename file or directory: {}", ec.message());
				}
				else {
					szgInformation("Renamed file or directory: {} -> {}", oldPath.string(), newPath.string());
					selectFileName = newFileName;
				}
			}
			isRenaming = false;
			newFileName.clear();
		}
	}
	else {
		ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + iconSize); // アイコン幅で折り返し
		i32 maxLabelLength = static_cast<i32>(iconSize / 2.6f);
		std::string label = fileNameString.size() > maxLabelLength ? fileNameString.substr(0, maxLabelLength - 3) + "..." : fileNameString; // 長すぎる場合は省略
		ImGui::Text(label.c_str());
	}

	// 次の行の位置を更新
	nextLinePosY = std::max(nextLinePosY, ImGui::GetCursorPosY() + padding.y);
}

void szg::EditorAssetBrowser::update_importer() {
	// 保存先ディレクトリの設定
	auto directory = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory / selectFileName;

	if (std::filesystem::is_directory(directory) && rootType == AssetRootType::Game) {
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

void szg::EditorAssetBrowser::update_shortcut() {
	// F2でリネーム開始
	if (ImGui::Shortcut(ImGuiKey_F2)) {
		if (!selectFileName.empty() && !isRenaming && rootType == AssetRootType::Game) {
			isRenaming = true;
			newFileName = selectFileName;
		}
	}

	// Deleteキーで削除
	if (ImGui::Shortcut(ImGuiKey_Delete)) {
		if (!selectFileName.empty() && rootType == AssetRootType::Game) {
			std::filesystem::path directory = ROOT_PATH[static_cast<i32>(rootType)] / currentDirectory / selectFileName;
			std::error_code ec;
			std::filesystem::remove_all(directory, ec);
			if (ec) {
				szgError("Failed to delete file or directory: {}", ec.message());
			}
			else {
				szgInformation("Deleted file or directory: {}", directory.string());
				selectFileName.clear();
			}
		}
	}
}
