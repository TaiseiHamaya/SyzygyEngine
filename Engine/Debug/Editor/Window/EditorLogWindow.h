#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "IEditorWindow.h"

#include <array>
#include <list>
#include <string>

#include <Library/Utility/Template/SingletonInterface.h>

#include "Engine/Application/Logger.h"

namespace szg {

class EditorLogWindow : public IEditorWindow, SingletonInterface<EditorLogWindow> {
	SZG_CLASS_SINGLETON(EditorLogWindow)

private:
	struct LogBody {
		Logger::Level level;
		std::string message;
	};

	struct LogState {
		bool isActive{ true };
		u64 numLogs{ 0 };
		string_literal icon;
		std::array<r32, 4> color;
	};

public:
	static void Allocate();
	static void Initialize(bool isActive_);

	static void Draw();
	static void DrawMenu(string_literal label);

	void draw() override;

	static void AppendLogEntry(Logger::Level level, const std::string& message);

private:
	std::array<LogState, Logger::LevelCount> logStates;
	bool isBottomScroll{ true };
	std::list<LogBody> logs;

	mutable std::mutex mutex;

private:
	static constexpr u64 MAX_LOG_SIZE = 4089;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
