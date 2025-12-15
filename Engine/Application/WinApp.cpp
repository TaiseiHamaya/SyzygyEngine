#include "WinApp.h"

using namespace szg;

#include <dbghelp.h>
#include <timeapi.h>

#include <Library/Utility/Tools/ConvertString.h>
#include <Library/Utility/Tools/RandomEngine.h>

#include "Engine/Application/ArgumentParser.h"
#include "Engine/Application/CrashHandler.h"
#include "Engine/Application/Logger.h"
#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/Assets/Audio/AudioManager.h"
#include "Engine/Assets/BackgroundLoader/BackgroundLoader.h"
#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"
#include "Engine/Assets/PrimitiveGeometry/PrimitiveGeometryAsset.h"
#include "Engine/Assets/PrimitiveGeometry/PrimitiveGeometryLibrary.h"
#include "Engine/Assets/Shader/ShaderLibrary.h"
#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/Debug/Editor/Window/EditorLogWindow.h"
#include "Engine/GraphicsAPI/DirectX/DxCore.h"
#include "Engine/Runtime/Clock/WorldClock.h"
#include "Engine/Runtime/Input/Input.h"
#include "Engine/Runtime/Scene/SceneManager2.h"
#include "Engine/Runtime/Input/TextInput.h"

#pragma comment(lib, "Dbghelp.lib") // Symとか
#pragma comment(lib, "Oleacc.lib") // GetProcessHandleFromHwnd
#pragma comment(lib, "winmm.lib") // timeBeginPeriod

extern "C" HANDLE WINAPI GetProcessHandleFromHwnd(_In_ HWND hwnd);

#ifdef DEBUG_FEATURES_ENABLE

#include "Engine/Debug/Editor/EditorMain.h"
#include "Engine/Debug/ImGui/ImGuiManager/ImGuiManager.h"

#include <pix_win.h>

#include <imgui.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // DEBUG_FEATURES_ENABLE

// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef DEBUG_FEATURES_ENABLE
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		return true;
#endif // _DEBUG
	// メッセージによって変える
	switch (msg) {
	case WM_DESTROY: //ウィンドウが破壊されたとき
		PostQuitMessage(0); // OSに通知
		return 0;
	}
	// 通常時は通常のメッセージ処理
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

WinApp::WinApp() noexcept = default;

WinApp::~WinApp() noexcept {
	// ログ
	szgInformation("Complete finalize application.");
	Logger::Finalize();
	// COMの終了
	CoUninitialize();
	// chrono内のTZDBを削除(これ以降ログ出力はされない)
	std::chrono::get_tzdb_list().~tzdb_list();
}

void WinApp::Initialize() {
	// ---------- ライブラリ、APIの初期化 ----------
#ifdef DEBUG_FEATURES_ENABLE
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG
	// chrono時間精度の設定
	timeBeginPeriod(1);

	szgCriticalIf(isInitialized, "WinApp is already initialized.");
	isInitialized = true;

	// アプリケーション内のwstring charsetをutf-8にする
	std::locale::global(std::locale("ja_JP.Utf-8"));

	// クラッシュハンドラの設定
	CrashHandler::Initialize();

	// COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// Log出力システムの初期化
	Logger::Initialize();
#ifdef DEBUG_FEATURES_ENABLE
	EditorLogWindow::Allocate();
#endif // DEBUG_FEATURES_ENABLE

	// ---------- Projectのロード ----------
	ProjectSettings::Initialize();

	// ---------- WindowsApplicationの起動 ----------
	// アプリケーションの初期化
	auto& instance = GetInstance();
	instance.initialize_application();
	// シンボルハンドラーの初期化
	SymInitialize(instance.hProcess, nullptr, true);

	ArgumentParser::Parse();

	// ---------- エンジン機能の初期化 ----------
	//DirectXの初期化
	DxCore::Initialize();
	// テクスチャマネージャの初期化
	TextureLibrary::Initialize();
	// 音関連の初期化
	AudioManager::Initialize();
	// 入力の初期化
	Input::Initialize();
	// 乱数エンジンの初期化
	RandomEngine::Initialize();
	// バックグラウンドローダーの初期化
	BackgroundLoader::Initialize();

#ifdef DEBUG_FEATURES_ENABLE
	ImGuiManager::Initialize();
#endif // _DEBUG

	// システム使用のアセットをロード
	// メッシュ
	PolygonMeshLibrary::RegisterLoadQue("[[szg]]/ErrorObject/ErrorObject.obj");
	PolygonMeshLibrary::RegisterLoadQue("[[szg]]/Grid/Grid.obj");
	PolygonMeshLibrary::RegisterLoadQue("[[szg]]/Camera/CameraAxis.obj");
	// Primitive
	PrimitiveGeometryLibrary::Transfer(
		"Ico3",
		std::make_shared<PrimitiveGeometryAsset>("[[szg]]/PrimitiveGeometry/Lighting/Ico3.json")
	);
	PrimitiveGeometryLibrary::Transfer(
		"Rect3D",
		std::make_shared<PrimitiveGeometryAsset>("[[szg]]/PrimitiveGeometry/Primitive/Rect3D.json")
	);

	// Shader
	ShaderLibrary::RegisterLoadQue("[[szg]]/Deferred/Mesh/StaticMesh.VS.hlsl");
	ShaderLibrary::RegisterLoadQue("[[szg]]/Deferred/Mesh/SkinningMesh.VS.hlsl");
	ShaderLibrary::RegisterLoadQue("[[szg]]/Deferred/Deferred.PS.hlsl");

	ShaderLibrary::RegisterLoadQue("[[szg]]/FullscreenShader.VS.hlsl");
	ShaderLibrary::RegisterLoadQue("[[szg]]/Misc/PrimitiveGeometry/PrimitiveGeometry.VS.hlsl");

	ShaderLibrary::RegisterLoadQue("[[szg]]/Deferred/Lighting/NonLighting.PS.hlsl");
	ShaderLibrary::RegisterLoadQue("[[szg]]/Deferred/Lighting/DirectionalLighting.PS.hlsl");
	ShaderLibrary::RegisterLoadQue("[[szg]]/Deferred/Lighting/PointLighting.PS.hlsl");

	ShaderLibrary::RegisterLoadQue("[[szg]]/Forward/Primitive/Rect3d.VS.hlsl");
	ShaderLibrary::RegisterLoadQue("[[szg]]/Forward/ForwardAlpha.PS.hlsl");

	ShaderLibrary::RegisterLoadQue("[[szg]]/Forward/Font/MsdfFont.VS.hlsl");
	ShaderLibrary::RegisterLoadQue("[[szg]]/Forward/Font/MsdfFont.PS.hlsl");

#ifdef DEBUG_FEATURES_ENABLE
	// デバッグ用アセットのロード
	// Shader
	ShaderLibrary::RegisterLoadQue("[[szg]]/Forward/Mesh/StaticMeshForward.VS.hlsl");
	ShaderLibrary::RegisterLoadQue("[[szg]]/Forward/Forward.PS.hlsl");
	ShaderLibrary::RegisterLoadQue("[[szg]]/Misc/PrimitiveGeometry/PrimitiveGeometry.PS.hlsl");

	// primitive
	PrimitiveGeometryLibrary::Transfer(
		"Sphere",
		std::make_shared<PrimitiveGeometryAsset>("[[szg]]/PrimitiveGeometry/Sphere.json")
	);
	PrimitiveGeometryLibrary::Transfer(
		"Box",
		std::make_shared<PrimitiveGeometryAsset>("[[szg]]/PrimitiveGeometry/Box.json")
	);
	PrimitiveGeometryLibrary::Transfer(
		"Line",
		std::make_shared<PrimitiveGeometryAsset>("[[szg]]/PrimitiveGeometry/Line.json")
	);
	PrimitiveGeometryLibrary::Transfer(
		"Frustum",
		std::make_shared<PrimitiveGeometryAsset>("[[szg]]/PrimitiveGeometry/Frustum.json")
	);

	// Texture
	TextureLibrary::RegisterLoadQue("./SyzygyEngine/EditorResources/Texture/EngineIcon_DirectionalLight.png");
#endif // _DEBUG

	// 待機
	BackgroundLoader::WaitEndExecute();

	SceneManager2::Initialize();

#ifdef DEBUG_FEATURES_ENABLE
	if (ArgumentParser::Contains("--editor-disable")) {
		EditorMain::SetActiveEditor(false);
	}
	EditorMain::Initialize();
#endif // DEBUG_FEATURES_ENABLE

	szgInformation("Complete initialize application.");
}

void WinApp::BeginFrame() {
#ifdef DEBUG_FEATURES_ENABLE
	Logger::SyncErrorWindow();

	auto& instance = GetInstance();
	instance.profiler.clear_timestamps();
	instance.profiler.timestamp("BeginFrame");

	WorldClock::Update();

	if (EditorMain::IsRuntimeInput()) {
		Input::Update();
	}

	DxCore::BeginFrame(); // SetDescriptorHeapsやる

	ImGuiManager::BeginFrame();

	PIXBeginEvent(DxCommand::GetCommandList().Get(), 0, "EditorScene");

	EditorMain::DrawBase(); // Editorのベース描画

	PIXEndEvent(DxCommand::GetCommandList().Get());

	SceneManager2::BeginFrame();
#else
	Logger::SyncErrorWindow();

	WorldClock::Update();

	Input::Update();

	DxCore::BeginFrame(); // SetDescriptorHeapsやる

	SceneManager2::BeginFrame();
#endif // DEBUG_FEATURES_ENABLE
}

void WinApp::Update() {
#ifdef DEBUG_FEATURES_ENABLE
	auto& instance = GetInstance();
	instance.profiler.timestamp("Update");
	if (IsStopUpdate()) { // 更新停止の場合
		return;
	}
#endif // DEBUG_FEATURES_ENABLE

	// シーン更新
	SceneManager2::Update();
}

void WinApp::Draw() {
#ifdef DEBUG_FEATURES_ENABLE
	auto& instance = GetInstance();
	instance.profiler.timestamp("PreDraw");

#else

#endif // DEBUG_FEATURES_ENABLE

	// 描画前処理
	SceneManager2::PreDraw();

#ifdef DEBUG_FEATURES_ENABLE
	instance.profiler.timestamp("Draw");

	PIXBeginEvent(DxCommand::GetCommandList().Get(), 0, "SceneRendering");
#endif // DEBUG_FEATURES_ENABLE

	// コマンドを積む
	SceneManager2::Draw();

#ifdef DEBUG_FEATURES_ENABLE
	PIXEndEvent(DxCommand::GetCommandList().Get());
#endif // DEBUG_FEATURES_ENABLE
}

void WinApp::EndFrame() {
	auto& instance = GetInstance();
#ifdef DEBUG_FEATURES_ENABLE
	instance.profiler.timestamp("EndFrame");

	// GUI
	ImGui::Begin("Application");
	ImGui::Checkbox("IsStopUpdate", &instance.isStopUpdate);
	instance.isPassedPause = false;
	if (ImGui::Button("NextFrame")) instance.isPassedPause = true;
	ImGui::SeparatorText("Profiler");
	instance.profiler.debug_gui();
	ImGui::End();

	// エディター描画
	PIXBeginEvent(DxCommand::GetCommandList().Get(), 0, "EditorScreen");
	EditorMain::Draw();
	PIXEndEvent(DxCommand::GetCommandList().Get());

	// ImGuiのコマンドを積む
	PIXBeginEvent(DxCommand::GetCommandList().Get(), 0, "ImGui");
	ImGuiManager::EndFrame();
	PIXEndEvent(DxCommand::GetCommandList().Get());
#endif // DEBUG_FEATURES_ENABLE

	// 描画実行とWait
	DxCore::EndFrame();

	// シーンのフレーム後処理
	SceneManager2::EndFrame();
	instance.wait_frame();
}

void WinApp::Finalize() {
	// 終了通知
	szgInformation("End Program.");
	// windowを閉じる
	CloseWindow(GetInstance().hWnd);
	szgInformation("Closed Window.");

	// 各種終了処理
	// Initializeと逆順でやる
	// シーン
	SceneManager2::Finalize();
#ifdef DEBUG_FEATURES_ENABLE
	// ImGui
	EditorMain::Finalize();
	ImGuiManager::Finalize();
#endif // DEBUG_FEATURES_ENABLE

	BackgroundLoader::Finalize();

	AudioManager::Finalize();

	TextureLibrary::Finalize();
	//DirectXを終了
	DxCore::Finalize();
}

void WinApp::ShowAppWindow() {
	// ウィンドウ表示
	if (!ProjectSettings::GetApplicationSettingsImm().hideWindowForce) {
		ShowWindow(GetInstance().hWnd, SW_SHOW);
		szgInformation("Show application window.");
	}

#ifdef DEBUG_FEATURES_ENABLE
	EditorMain::Setup();
#endif // DEBUG_FEATURES_ENABLE

	// 時計初期化
	WorldClock::Initialize();
}

void WinApp::ProcessMessage() {
	auto& instance = GetInstance();
	TextInput::BeginFrame();
	while (true) {
		// windowにメッセージが来たら最優先で処理
		if (PeekMessage(&instance.msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&instance.msg);
			DispatchMessage(&instance.msg);
		}
		else {
			return;
		}
		switch (instance.msg.message) {
		case WM_QUIT: // windowの×ボタンが押されたら通知
			instance.isEndApp = true;
			break;
		case WM_CHAR:
			TextInput::ProssesInput(
				static_cast<wchar_t>(instance.msg.wParam),
				static_cast<u32>(instance.msg.lParam)
			);
			break;
		}
	}
}

bool WinApp::IsEndApp() noexcept {
	if (GetInstance().isEndApp) { // ×ボタンが押されたら終わる
		return true;
	}
	if (SceneManager2::IsEndProgram()) {
		return true;
	}
#ifdef DEBUG_FEATURES_ENABLE
	if (EditorMain::IsEndApplicationForce()) {
		return true;
	}
#endif // DEBUG_FEATURES_ENABLE
	return false;
}

HWND WinApp::GetWndHandle() noexcept {
	return GetInstance().hWnd;
}

HANDLE WinApp::GetProcessHandle() noexcept {
	return GetInstance().hProcess;
}

HINSTANCE WinApp::GetInstanceHandle() noexcept {
	return GetInstance().hInstance;
}

void WinApp::initialize_application() {
	std::wstring windowTitleW = ConvertString(ProjectSettings::GetProjectName());

	// ウィンドウの設定
	WNDCLASS windowClass{};
	windowClass.lpfnWndProc = WindowProc;// ウィンドウプロシージャ
	windowClass.lpszClassName = windowTitleW.data();
	windowClass.hInstance = GetModuleHandle(nullptr); // インスタンスハンドル
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウを登録
	RegisterClass(&windowClass);

	// ウィンドウサイズ指定用に構造体にする
	RECT wrc = { 0,0,
		static_cast<LONG>(ProjectSettings::ClientWidth()), static_cast<LONG>(ProjectSettings::ClientHeight()) };
	// 実際にwrcを変更
	AdjustWindowRect(&wrc, ProjectSettings::WindowStyle(), false);

	// ウィンドウの生成
	hWnd = CreateWindowW(
		windowClass.lpszClassName,
		windowTitleW.data(),
		ProjectSettings::WindowStyle(),
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr
	);

	hInstance = windowClass.hInstance;

	hProcess = GetProcessHandleFromHwnd(hWnd);
}

#include <thread>

void WinApp::wait_frame() {
	if (!ProjectSettings::GetApplicationSettingsImm().maxFrameRate.has_value()) {
		return;
	}
	using millisecond_f = std::chrono::duration<r32, std::milli>;

	const u32 targetFPS = ProjectSettings::GetApplicationSettingsImm().maxFrameRate.value() + 5;
	const millisecond_f MinCheckTime{ 1000.00000f / targetFPS }; // 少し短い時間を使用する
	// 開始
	auto& begin = WorldClock::BeginTime();
	// 今
	auto now = std::chrono::high_resolution_clock::now();
	// 経過時間
	auto duration = std::chrono::duration_cast<millisecond_f>(now - begin);
	// 基準より短い場合
	if (duration < MinCheckTime) {
		// 残りの止める時間
		millisecond_f sleepMilliSec = MinCheckTime - duration;
		// 止める
		std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::microseconds>(sleepMilliSec));
	}
}

#ifdef DEBUG_FEATURES_ENABLE

bool WinApp::IsStopUpdate() {
	auto& instance = GetInstance();
	return instance.isStopUpdate && !instance.isPassedPause;
}

#endif // DEBUG_FEATURES_ENABLE
