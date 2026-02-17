#pragma once

#include <array>
#include <filesystem>
#include <format>

#include <Library/Utility/Tools/ChronoUtility.h>

namespace EngineSettings {
// UseRenderingEngine
#define SZG_RENDERING_ENGINE_DIRECTX12
//#define SZG_RENDERING_ENGINE_VULKAN

/// <summary>
/// ログファイル名
/// </summary>
extern inline const std::filesystem::path LogFileName{
	std::format(L"{:%F-%H%M%S}.log", ChronoUtility::NowLocalSecond())
};

/// <summary>
/// ログファイルディレクトリ
/// </summary>
extern inline const std::filesystem::path LogFilePath{
	std::format(L"./Log/{}", EngineSettings::LogFileName.native())
};

#ifdef DEBUG_FEATURES_ENABLE
/// <summary>
/// <para>ログレベルの設定</para>
/// <para>詳細はLogger.hを参照</para>
/// </summary>
static constexpr std::array<u8, 6> LoggerConfigs = {
	0b00000111, // Trace
	0b00000111, // Information
	0b00000111, // Warning
	0b00111111, // Error
	0b00111111, // Critical
	0b00101111, // Assert
};
#else
/// <summary>
/// <para>ログレベルの設定</para>
/// <para>詳細はLogger.hを参照</para>
/// </summary>
static constexpr std::array<u8, 6> LoggerConfigs = {
	0b00000000, // Trace
	0b00000000, // Information
	0b00000111, // Warning
	0b00111111, // Error
	0b00111111, // Critical
	0b00000000, // Assert
};
#endif // DEBUG_FEATURES_ENABLE
};
