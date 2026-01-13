#include "ProjectSettings.h"

using namespace szg;

#include <fstream>

#include <json.hpp>

#include "Engine/Application/Logger.h"
#include "Engine/GraphicsAPI/RenderingSystemValues.h"

#define COLOR_RGBA_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

constexpr u32 DEFAULT_WINDOW_STYLE = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;

void ProjectSettings::Initialize() {
	ProjectSettings& instance = GetInstance();

	instance.graphicsSettings = {
		2,
		{6,6},
		65536,
		20,
		1,
	};

	nlohmann::json json{};

	std::ifstream ifstream{ "./Game/Core/Core.json" };

	if (ifstream.fail()) {
		szgError("Project is not found.");
		return;
	}

	json = nlohmann::json::parse(ifstream);

	instance.projectName = json["ProjectName"].get<std::string>();
	if (json.contains("WindowStyle")) {
		nlohmann::json& jsonWindowStyle = json["WindowStyle"];
		instance.windowStyle = 0;
		bool isFullscreen = false;
		// フルスクリーン
		if (jsonWindowStyle.value("Fullscreen", false)) {
			instance.windowStyle = WS_POPUP;
			isFullscreen = true;
		}
		else {
			instance.windowStyle = DEFAULT_WINDOW_STYLE;
		}
		if (!isFullscreen) {
			// 最大化ボタン
			if (jsonWindowStyle.value("MaximizeBox", false)) {
				instance.windowStyle |= WS_MAXIMIZEBOX;

			}
			else {
				instance.windowStyle &= ~WS_MAXIMIZEBOX;
			}

			// 最小化ボタン
			if (jsonWindowStyle.value("MinimizeBox", true)) {
				instance.windowStyle |= WS_MINIMIZEBOX;
			}
			else {
				instance.windowStyle &= ~WS_MINIMIZEBOX;
			}

			// サイズ変更可能
			if (jsonWindowStyle.value("Resizable", false)) {
				instance.windowStyle |= WS_THICKFRAME;
			}
			else {
				instance.windowStyle &= ~WS_THICKFRAME;
			}

			// 最大化状態で起動
			if (jsonWindowStyle.value("MaximizeDefault", false)) {
				instance.windowStyle |= WS_MAXIMIZE;
			}
			else {
				instance.windowStyle &= ~WS_MAXIMIZE;
			}
		}
	}

	// DirectX設定
	if (json.contains("Graphics")) {
		nlohmann::json& graphics = json["Graphics"];
		// バッファリング数
		instance.graphicsSettings.numBuffering = graphics.value("NumBuffering", 2);
		// シェーダーバージョン
		if (graphics.contains("ShaderVersion")) {
			std::string ver = graphics["ShaderVersion"].get<std::string>();
			instance.graphicsSettings.shaderVersion = {
				ver[0] - '0',ver[2] - '0'
			};
		}
		// DescriptorHeapの数
		instance.graphicsSettings.srvHeapSize = graphics.value("SrvHeapSize", 65536);
		instance.graphicsSettings.rtvHeapSize = graphics.value("RtvHeapSize", 20);
		instance.graphicsSettings.dsvHeapSize = graphics.value("DsvHeapSize", 1);
		// クリアカラー
		instance.graphicsSettings.clearColor = graphics.value("ClearColor", ColorRGBA{ 0.39f, 0.58f, 0.92f, 1.0f });
	}

	// アプリケーション設定
	if (json.contains("Application")) {
		nlohmann::json& appSettings = json["Application"];
		if (appSettings.contains("Window")) {
			nlohmann::json& windowSettings = appSettings["Window"];
			instance.applicationSettings.clientWidth = windowSettings.value("Width", 1280);
			instance.applicationSettings.clientHeight = windowSettings.value("Height", 720);
		}
		if (appSettings.contains("FixDeltaSeconds") && appSettings["FixDeltaSeconds"].is_number_float()) {
			instance.applicationSettings.fixDeltaSeconds = appSettings["FixDeltaSeconds"].get<r32>();
		}
		if (appSettings.contains("MaxFrameRate") && appSettings["MaxFrameRate"].is_number()) {
			instance.applicationSettings.maxFrameRate = appSettings["MaxFrameRate"].get<u32>();
		}
	}
}

const std::string& ProjectSettings::GetProjectName() {
	return GetInstance().projectName;
}

u32 ProjectSettings::WindowStyle() {
	return GetInstance().windowStyle;
}

const ProjectSettings::Application& ProjectSettings::GetApplicationSettingsImm() noexcept {
	return GetInstance().applicationSettings;
}

const ProjectSettings::Graphics& ProjectSettings::GetGraphicsSettingsImm() noexcept {
	return GetInstance().graphicsSettings;
}

ProjectSettings::Application& ProjectSettings::GetApplicationSettingsMut() noexcept {
	return GetInstance().applicationSettings;
}

ProjectSettings::Graphics& ProjectSettings::GetGraphicsSettingsMut() noexcept {
	return GetInstance().graphicsSettings;
}

Vector2 ProjectSettings::ClientSize() noexcept {
	auto& instance = GetInstance();
	return Vector2{
		static_cast<r32>(instance.applicationSettings.clientWidth),
		static_cast<r32>(instance.applicationSettings.clientHeight)
	};
}

u32 ProjectSettings::ClientWidth() noexcept {
	return static_cast<u32>(GetInstance().applicationSettings.clientWidth);
}

u32 ProjectSettings::ClientHeight() noexcept {
	return static_cast<u32>(GetInstance().applicationSettings.clientHeight);
}

void ProjectSettings::SetMaxFrameRate(std::optional<u32> maxFrameRate) noexcept {
	GetInstance().applicationSettings.maxFrameRate = maxFrameRate;
}
