#pragma once

#include <array>
#include <format>
#include <fstream>
#include <mutex>
#include <source_location>
#include <string>

#include <Library/Utility/Template/bitflag.h>
#include <Library/Utility/Template/SingletonInterface.h>

namespace szg {

class Logger : public SingletonInterface<Logger> {
	SZG_CLASS_SINGLETON(Logger)

public:
	/// <summary>
	/// ログレベル
	/// </summary>
	enum class Level : u8 {
		Trace,
		Information,
		Warning,
		Error,
		Critical,
		Assert,
	};

	static constexpr u8 LevelCount = 6;

	static constexpr std::array<wstring_literal, LevelCount> LevelStringW = {
		L"Trace",
		L"Information",
		L"Warning",
		L"Error",
		L"Critical",
		L"Assert",
	};

	/// <summary>
	/// ログ出力ビットフラグ
	/// </summary>
	enum class OutputDestination : u8 {
		Console = 1 << 0,
		File = 1 << 1,
		Editor = 1 << 2,
		Window = 1 << 3,
		StackTrace = 1 << 4,
		Breakpoint = 1 << 5,
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	static void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	static void Finalize();

	/// <summary>
	/// エラーウィンドウ
	/// </summary>
	static void SyncErrorWindow();

	/// <summary>
	/// ログエントリポイント
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="sourceLocation"></param>
	/// <param name="level">ログレベル</param>
	/// <param name="msg">ログメッセージのformat_string</param>
	/// <param name="...args"></param>
	template<typename ...Args>
	static void LogEntryPoint(const std::source_location& sourceLocation, Level level, std::format_string<Args...> msg, Args&& ...args);

	/// <summary>
	/// ログエントリポイント
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="sourceLocation"></param>
	/// <param name="level">ログレベル</param>
	/// <param name="msg">ログメッセージのformat_string</param>
	/// <param name="...args"></param>
	template<typename ...Args>
	static void LogEntryPoint(const std::source_location& sourceLocation, Level level, std::wformat_string<Args...> msg, Args&& ...args);

private:
	// ログ本体
	void body(const std::wstring& file, Level level, const std::wstring& message);

	// 中間処理
	void intermediate_a(const std::source_location& sourceLocation, Level level, const std::string& message);
	void intermediate_w(const std::source_location& sourceLocation, Level level, const std::wstring& message);

	// コンソール出力
	void output_console(const std::wstring& msg);
	// ファイル出力
	void output_file(const std::wstring& msg);
	// ポップアップウィンドウ出力
	void popup_window(Level level, const std::wstring& caption, const std::wstring& msg);
	// スタックトレース出力
	void stack_trace();
	// スタックトレース用の1行出力
	void stack_trace_line(const std::wstring& msg);

private:
	bool canOutputLog{ false };
	std::mutex mutex;
	std::wofstream logFile;
};

SZG_BITFLAG(Logger::OutputDestination)

template<typename ...Args>
inline void Logger::LogEntryPoint(const std::source_location& sourceLocation, Level level, std::format_string<Args...> msg, Args && ...args) {
	std::string message = std::format(msg, std::forward<Args>(args)...);
	Logger::GetInstance().intermediate_a(sourceLocation, level, message);
}

template<typename ...Args>
inline void Logger::LogEntryPoint(const std::source_location& sourceLocation, Level level, std::wformat_string<Args...> msg, Args && ...args) {
	std::wstring message = std::format(msg, std::forward<Args>(args)...);
	Logger::GetInstance().intermediate_w(sourceLocation, level, message);
}

}; // szg

// Traceレベルのログ出力
#define szgTrace(msg, ...) szg::Logger::LogEntryPoint(std::source_location::current(), szg::Logger::Level::Trace, msg __VA_OPT__(,) __VA_ARGS__)
// Informationレベルのログ出力
#define szgInformation(msg, ...) szg::Logger::LogEntryPoint(std::source_location::current(), szg::Logger::Level::Information, msg __VA_OPT__(,) __VA_ARGS__)
// Warningレベルのログ出力
#define szgWarning(msg, ...) szg::Logger::LogEntryPoint(std::source_location::current(), szg::Logger::Level::Warning, msg __VA_OPT__(,) __VA_ARGS__)
// Errorレベルのログ出力
#define szgError(msg, ...) szg::Logger::LogEntryPoint(std::source_location::current(), szg::Logger::Level::Error, msg __VA_OPT__(,) __VA_ARGS__)
// Criticalレベルのログ出力
#define szgCritical(msg, ...) szg::Logger::LogEntryPoint(std::source_location::current(), szg::Logger::Level::Critical, msg __VA_OPT__(,) __VA_ARGS__)

// 条件付きWarningログ出力
#define szgWarningIf(conditional, msg, ...) if((conditional)) szgWarning(msg __VA_OPT__(,) __VA_ARGS__)
// 条件付きErrorログ出力
#define szgErrorIf(conditional, msg, ...) if((conditional)) szgError(msg __VA_OPT__(,) __VA_ARGS__)
// 条件付きCriticalログ出力
#define szgCriticalIf(conditional, msg, ...) if((conditional)) szgCritical(msg __VA_OPT__(,) __VA_ARGS__)

// Assert
#define szgAssert(conditional) if (!(conditional)) szg::Logger::LogEntryPoint(std::source_location::current(), szg::Logger::Level::Assert, L"Assertion failed: {}", L""#conditional)
