#pragma once

#include "../IEditorWindow.h"

#include <array>
#include <filesystem>

#include <Library/Math/Vector2.h>

#include "./EditorAssetPackageLoader.h"

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

private:
	enum class AssetRootType {
		UNSELECT,
		ENGINE,
		GAME,

		MAX,
	};

private:
	AssetRootType rootType{ AssetRootType::GAME };
	r32 iconSize{ 64 };

	std::filesystem::path currentDirectory;
	std::string selectFileName;

	EditorAssetPackageLoader assetPackageLoader;

	Vector2 lastCursorPos;
	r32 nextLinePosY;

private:
	static constexpr std::array<string_literal, static_cast<i32>(AssetRootType::MAX)>  ROOT_TAG{
		"[[Unselect]]",
		"[[SZG]]",
		"[[Game]]",
	};

	static constexpr std::array<string_literal, static_cast<i32>(AssetRootType::MAX)>  ROOT_PATH{
		"",
		"./SyzygyEngine/EngineResources",
		"./Game/Assets",
	};
};

}; // namespace szg
