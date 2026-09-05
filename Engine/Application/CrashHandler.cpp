#include "CrashHandler.h"

#include <Windows.h>

#include <DbgHelp.h>
#include <strsafe.h>

#include "Engine/Application/EngineSettings.h"
#include "Engine/Application/Logger.h"

using namespace szg;

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception) noexcept {
	Logger::Finalize();

	SYSTEMTIME time;
	GetLocalTime(&time);

	// ---------- 前準備 ----------
	// ディレクトリ作成
	CreateDirectory(L"./Log/Dumps", nullptr);

	// dmpファイルのパス
	wchar_t dumpFilePath[MAX_PATH]{ 0 };
	StringCchPrintfW(dumpFilePath, MAX_PATH, L"./Log/Dumps/%04d-%02d%02d-%02d%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	// ---------- dumpファイルを作成 ----------
	// ファイル生成
	HANDLE dumpFileHandle = CreateFile(dumpFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	DWORD processId = GetCurrentProcessId(); // 後々必要
	DWORD threadId = GetCurrentThreadId();

	MINIDUMP_EXCEPTION_INFORMATION minidumpInfo{ 0 }; // minidumpのメイン
	minidumpInfo.ThreadId = threadId;
	minidumpInfo.ExceptionPointers = exception;
	minidumpInfo.ClientPointers = TRUE;

	const std::wstring& logFileName = EngineSettings::LogFilePath.native();

	MINIDUMP_USER_STREAM stream{ 0 }; // 追加コメント
	stream.Type = CommentStreamW;
	stream.BufferSize = static_cast<ULONG>(logFileName.size() * sizeof(wchar_t));
	stream.Buffer = PVOID(logFileName.data());

	MINIDUMP_USER_STREAM_INFORMATION streamInfo{ 0 }; // まとめる
	streamInfo.UserStreamArray = &stream;
	streamInfo.UserStreamCount = 1;

	// dumpファイルの書き込み
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInfo, &streamInfo, nullptr);
	CloseHandle(dumpFileHandle);

	// ---------- メッセージボックスの表示 ----------
	// 絶対パスを取得
	wchar_t fullDumpPath[MAX_PATH]{ 0 };
	GetFullPathNameW(dumpFilePath, MAX_PATH, fullDumpPath, nullptr);
	wchar_t fullLogPath[MAX_PATH]{ 0 };
	GetFullPathNameW(logFileName.c_str(), MAX_PATH, fullLogPath, nullptr);

	// メッセージ構築
	wchar_t message[2048] = {};
	StringCchPrintfW(message, 2048,
		L"致命的なエラーが発生したため、アプリケーションを終了しました。\n"
		L"サポートが必要な場合は開発者に以下のファイルを送信してください。\n\n"
		L"%s\n%s", fullDumpPath, fullLogPath);

	// メッセージボックス表示
	MessageBoxW(nullptr, message, L"Critical error", MB_ICONERROR | MB_OK | MB_SYSTEMMODAL);

	return EXCEPTION_EXECUTE_HANDLER;
}

namespace szg {

namespace CrashHandler {

void Initialize() {
	SetUnhandledExceptionFilter(ExportDump);
}

} // namespace CrashHandler

} // namespace szg
