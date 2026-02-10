#pragma once

#include "../IEditorWindow.h"

#include <filesystem>

#include <Library/Math/Vector2.h>

#include "./EditorAssetImporter.h"
#include "Engine/Assets/AssetRootPath.h"

namespace szg {

class EditorAssetBrowser final : public szg::IEditorWindow {
public:
	void draw() override;

	void on_drop_file(const std::filesystem::path& filePath);

private:
	// ヘッダー
	void draw_header();

	// フォルダの描画
	void draw_assets();

	// フッター
	void draw_footer();

	// 右クリックメニューの表示
	void draw_right_click_menu();

	// Drag＆Dropの処理
	void handle_drag_and_drop();

	// ファイルの内容を描画
	void draw_file_content(const std::string& name, bool isDirectory, i32 idx);

	void update_importer();

	void update_shortcut();

private:
	AssetRootType rootType{ AssetRootType::Game };
	r32 iconSize{ 64 };

	std::filesystem::path currentDirectory;
	std::string selectFileName;

	EditorAssetImporter assetImporter;

	Vector2 lastCursorPos;
	r32 nextLinePosY;

	bool isRenaming{ false };
	std::string newFileName;
};

}; // namespace szg
