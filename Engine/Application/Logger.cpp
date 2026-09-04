#include "Logger.h"

#include <Windows.h>

#include <dbghelp.h>

#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <mutex>
#include <vector>

#include <Library/Utility/Tools/ChronoUtility.h>
#include <Library/Utility/Tools/ConvertString.h>

#include "./WinApp.h"
#include "Engine/Application/EngineSettings.h"
#include "Engine/Debug/Editor/Window/Logger/EditorLogWindow.h"

using namespace szg;

static std::wstring FormatLocationW(const std::source_location& sourceLocation) {
	std::string_view funcName = sourceLocation.function_name();

	funcName.remove_suffix(funcName.size() - funcName.find_last_of('('));
	if (funcName.ends_with("noexcept")) {
		funcName.remove_suffix(funcName.size() - funcName.find_last_of('('));
	}

	funcName.remove_prefix(funcName.find_last_of(' ') + 1);
	return ConvertString(funcName) + std::format(L" | Line:{}", sourceLocation.line());
}

void Logger::Initialize() {
	auto& instance = GetInstance();
	std::filesystem::create_directory(EngineSettings::LogFilePath.parent_path());
	instance.logFile.exceptions(std::ios_base::badbit | std::ios_base::failbit);
	instance.logFile.open(EngineSettings::LogFilePath);

	std::lock_guard<std::mutex> lock{ instance.mutex };
	instance.canOutputLog = true;
}

void Logger::Finalize() {
	auto& instance = GetInstance();
	std::lock_guard<std::mutex> lock{ instance.mutex };
	instance.canOutputLog = false;
	instance.logFile.close();
}

void Logger::SyncErrorWindow() {
	auto& instance = GetInstance();
	std::lock_guard<std::mutex> lock{ instance.mutex };
	instance.logFile.flush();
}

void Logger::body(const std::wstring& file, Level level, const std::wstring& message) {
	std::lock_guard<std::mutex> lock{ mutex };

	ChronoUtility::LocalTimeSeconds time = ChronoUtility::NowLocalSecond();

	eps::bitflag<OutputDestination> config = EngineSettings::LoggerConfigs[static_cast<u8>(level)];

	std::wstring levelStringW = LevelStringW[static_cast<u8>(level)];
	std::wstring out =
		std::format(L"{:%H:%M:%S} | {: <11} | {} | {}\n", time, levelStringW, message, file);

	// コンソール出力
	if (config & OutputDestination::Console) {
		output_console(out);
	}
	// ファイル出力
	if (config & OutputDestination::File) {
		output_file(out);
	}
	// Stacktraceの出力
	if (config & OutputDestination::StackTrace) {
		stack_trace();
	}
	// Editor出力
	if (config & OutputDestination::Editor) {
#ifdef DEBUG_FEATURES_ENABLE
		EditorLogWindow::AppendLogEntry(level, ConvertString(std::format(L"{:%H:%M:%S} | {}\n", time, message)));
#endif // DEBUG_FEATURES_ENABLE
	}
	// ウィンドウ出力
	if (config & OutputDestination::Window) {
		popup_window(level, std::format(L"[{}] {}", levelStringW, file), message);
	}
	// ブレークポイントによって停止させる
	if (config & OutputDestination::Breakpoint) {
#ifdef DEBUG_FEATURES_ENABLE
		__debugbreak();
#else
		throw std::runtime_error("致命的なエラーが発生したためアプリケーションを停止しました。");
#endif // DEBUG_FEATURES_ENABLE
	}
}

void Logger::intermediate_a(const std::source_location& sourceLocation, Level level, const std::string& message) {
	if (!canOutputLog) {
		throw std::runtime_error("アプリケーション終了後にログ出力が行われました。");
		return;
	}

	body(FormatLocationW(sourceLocation), level, ConvertString(message));
}

void Logger::intermediate_w(const std::source_location& sourceLocation, Level level, const std::wstring& message) {
	if (!canOutputLog) {
		throw std::runtime_error("アプリケーション終了後にログ出力が行われました。");
		return;
	}

	body(FormatLocationW(sourceLocation), level, message);
}

void Logger::output_console(const std::wstring& msg) {
	OutputDebugStringW(msg.c_str());
}

void Logger::output_file(const std::wstring& msg) {
	auto& instance = GetInstance();
	instance.logFile << msg;
}

void Logger::popup_window(Level level, const std::wstring& caption, const std::wstring& msg) {
	UINT flag = 0;
	switch (level) {
	case Logger::Level::Trace:
	case Logger::Level::Information:
		flag = MB_ICONINFORMATION;
		break;
	case Logger::Level::Warning:
		flag = MB_ICONWARNING;
		break;
	case Logger::Level::Error:
	case Logger::Level::Critical:
	case Logger::Level::Assert:
		flag = MB_ICONERROR;
		break;
	default:
		break;
	}

	flag |= MB_OK | MB_SYSTEMMODAL;
	MessageBoxW(nullptr, msg.c_str(), caption.c_str(), flag);
}

void Logger::stack_trace() {
	std::array<void*, 100> stacks{};
	WORD numberOfFrames;
	HANDLE process = WinApp::GetProcessHandle();

	numberOfFrames = RtlCaptureStackBackTrace(0, 100, stacks.data(), nullptr);

	std::vector<char> symbolBuffer(sizeof(SYMBOL_INFO) + 256);
	SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer.data());

	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	stack_trace_line(L"\nOutput stack trace.\n");

#ifdef DEBUG_FEATURES_ENABLE
	constexpr u32 NumSkipTrace{ 4 };
#else
	constexpr u32 NumSkipTrace{ 3 };
#endif // _DEBUG

	for (u32 i = NumSkipTrace; i < numberOfFrames; i++) {
		SymFromAddr(process, reinterpret_cast<DWORD64>(stacks[i]), 0, symbol);
		std::wstring output = std::format(L"{: >2} | 0x{:016x} | {}\n", i - NumSkipTrace, symbol->Address, ConvertString(symbol->Name));
		stack_trace_line(output);
	}

	stack_trace_line(L"\n");
}

void Logger::stack_trace_line(const std::wstring& msg) {
	output_file(msg);
	output_console(msg.c_str());
}
