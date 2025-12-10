#pragma once

#include <windows.h>

#include <Library/Utility/Template/SingletonInterface.h>

#include "Engine/Debug/Profiler/TimestampProfiler.h"

namespace szg {

class WinApp final : public SingletonInterface<WinApp> {
	friend class SingletonInterface<WinApp>;

private:
	WinApp() noexcept;
	~WinApp() noexcept;

public:
	WinApp(const WinApp&) = delete;
	WinApp& operator=(const WinApp&) = delete;
	WinApp(WinApp&&) = delete;
	WinApp& operator=(WinApp&&) = delete;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	static void Initialize();

	/// <summary>
	/// フレーム開始処理
	/// </summary>
	static void BeginFrame();

	/// <summary>
	/// 更新
	/// </summary>
	static void Update();

	/// <summary>
	/// 描画
	/// </summary>
	static void Draw();

	/// <summary>
	/// フレーム終了処理
	/// </summary>
	static void EndFrame();
	

	/// <summary>
	/// 終了処理
	/// </summary>
	static void Finalize();

public:
	/// <summary>
	/// ウィンドウ表示
	/// </summary>
	static void ShowAppWindow();

	/// <summary>
	/// プロセスメッセージの取得
	/// </summary>
	static void ProcessMessage();

public:
	static bool IsEndApp() noexcept;
	static HWND GetWndHandle() noexcept;
	static HANDLE GetProcessHandle() noexcept;
	static HINSTANCE GetInstanceHandle() noexcept;

private:
	void initialize_application();

	void wait_frame();

private:
	static inline bool isInitialized{ false };

private:
	bool isEndApp{ false };
	HWND hWnd{ nullptr };
	HINSTANCE hInstance{ nullptr };
	HANDLE hProcess{ nullptr };

	MSG msg{};

#ifdef DEBUG_FEATURES_ENABLE
public:
	static bool IsStopUpdate();

private:
	bool isStopUpdate{ false };
	bool isPassedPause{ false };
	mutable TimestampProfiler profiler{};
#endif // _DEBUG
};


}; // szg
