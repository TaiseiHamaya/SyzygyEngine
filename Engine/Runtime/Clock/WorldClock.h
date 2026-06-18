#pragma once

#include <chrono>

#include <Library/Utility/Template/SingletonInterface.h>

#ifdef DEBUG_FEATURES_ENABLE
#include "Engine/Debug/Profiler/WorldClockProfiler.h"
#endif // _DEBUG

namespace szg {

class WorldClock final : public SingletonInterface<WorldClock> {
	SZG_CLASS_SINGLETON(WorldClock)

public:
	using Clock = std::chrono::steady_clock;
	using SysClock = std::chrono::system_clock;

public:
	static void Initialize();
	static void Update();

public:
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	static r32 DeltaSeconds();

	/// <summary>
	/// フレーム開始時刻
	/// 起点は不定
	/// </summary>
	/// <returns></returns>
	static const Clock::time_point& FrameTime();

	/// <summary>
	/// フレーム開始時刻
	/// 起点はstd::chrono::system_clockに準拠
	/// </summary>
	/// <returns></returns>
	static const SysClock::time_point& FrameSysTime();

	/// <summary>
	/// アプリケーション開始時刻
	/// </summary>
	static const SysClock::time_point& AppLaunchTime();

private:
	r32 deltaSeconds{ 0.0f };
	Clock::time_point frameTime;
	SysClock::time_point frameSysTime;

	SysClock::time_point appLaunchTime;

#ifdef DEBUG_FEATURES_ENABLE
public:
	static void DebugGui();

private:
	WorldClockProfiler profiler;
#endif // _DEBUG
};

}; // szg
