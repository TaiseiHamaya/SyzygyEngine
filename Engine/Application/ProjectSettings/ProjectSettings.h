#pragma once

#include <optional>
#include <string>

#include <Library/Math/ColorRGBA.h>
#include <Library/Math/Vector2.h>
#include <Library/Utility/template/SingletonInterface.h>

/// <summary>
/// プロジェクト設定
/// </summary>
namespace szg {

class ProjectSettings final : public SingletonInterface<ProjectSettings> {
	SZG_CLASS_SINGLETON(ProjectSettings)

public:
	/// <summary>
	/// アプリケーション設定
	/// </summary>
	struct Application {
		u32 clientWidth{ 1280 };
		u32 clientHeight{ 720 };

		std::optional<r32> fixDeltaSeconds{ std::nullopt };

		std::optional<u32> maxFrameRate{ std::nullopt };
	
		bool hideWindowForce{ false };
	};

	/// <summary>
	/// グラフィック設定
	/// </summary>
	struct Graphics {
		u32 numBuffering{ 2 };
		std::pair<u32, u32> shaderVersion{ 6, 6 };
		u32 srvHeapSize{ 65536 };
		u32 rtvHeapSize{ 20 };
		u32 dsvHeapSize{ 1 };
		ColorRGBA clearColor{ 0.39f, 0.58f, 0.92f, 1.0f };
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	static void Initialize();

	/// <summary>
	/// プロジェクト名の取得
	/// </summary>
	/// <returns></returns>
	static const std::string& GetProjectName();

	/// <summary>
	/// WindowStyleの取得
	/// </summary>
	/// <returns></returns>
	static u32 WindowStyle();

public: // getter

	/// <summary>
	/// アプリケーション設定の取得
	/// </summary>
	/// <returns></returns>
	static const Application& GetApplicationSettingsImm() noexcept;

	/// <summary>
	/// グラフィックス設定の取得
	/// </summary>
	/// <returns></returns>
	static const Graphics& GetGraphicsSettingsImm() noexcept;

	/// <summary>
	/// アプリケーション設定の可変参照取得
	/// </summary>
	/// <returns></returns>
	static Application& GetApplicationSettingsMut() noexcept;

	/// <summary>
	/// グラフィック設定の可変参照取得
	/// </summary>
	/// <returns></returns>
	static Graphics& GetGraphicsSettingsMut() noexcept;

	/// <summary>
	/// ウィンドウのクライアントサイズをVector2で取得
	/// </summary>
	/// <returns></returns>
	static Vector2 ClientSize() noexcept;

	/// <summary>
	/// ウィンドウのクライアント幅を取得
	/// </summary>
	/// <returns></returns>
	static u32 ClientWidth() noexcept;

	/// <summary>
	/// ウィンドウのクライアントの高さを取得
	/// </summary>
	/// <returns></returns>
	static u32 ClientHeight() noexcept;

	/// <summary>
	/// 最大フレームレートを設定
	/// nulloptは無制限
	/// </summary>
	/// <param name="maxFrameRate"></param>
	static void SetMaxFrameRate(std::optional<u32> maxFrameRate) noexcept;

private:
	std::string projectName;
	u32 windowStyle{ 0 };

	Application applicationSettings;
	Graphics graphicsSettings;
};

}; // szg
