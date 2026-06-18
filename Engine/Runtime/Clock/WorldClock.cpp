#include "WorldClock.h"

#include <ratio>
#include <thread>

#include "Engine/Application/ProjectSettings/ProjectSettings.h"

namespace chrono = std::chrono;
using Clock = chrono::steady_clock;
using SysClock = chrono::system_clock;

using namespace szg;

void WorldClock::Initialize() {
	auto& instance = GetInstance();
	instance.frameTime = Clock::now();
	instance.frameSysTime = SysClock::now();
	instance.appLaunchTime = instance.frameSysTime;
	std::this_thread::sleep_for(chrono::microseconds(1666));
	instance.deltaSeconds = ProjectSettings::GetApplicationSettingsImm().fixDeltaSeconds.value_or(0);
}

void WorldClock::Update() {
	// 少数型秒のusing
	using second_f = chrono::duration<r32, std::ratio<1>>;

	auto&& instance = GetInstance();

	// 現在時刻を取得
	auto now = Clock::now();
	instance.frameSysTime = SysClock::now();
	// duration算出
	auto secDuration = chrono::duration_cast<second_f>(now - instance.frameTime);
	// deltaTimeとして記録
	const auto& fixDeltaSeconds = ProjectSettings::GetApplicationSettingsImm().fixDeltaSeconds;
	instance.deltaSeconds = fixDeltaSeconds.has_value() ? std::min(fixDeltaSeconds.value(), secDuration.count()) : secDuration.count();

	// Startを更新
	instance.frameTime = now;

#ifdef DEBUG_FEATURES_ENABLE
	instance.profiler.update();
#endif // _DEBUG
}

r32 WorldClock::DeltaSeconds() {
	return GetInstance().deltaSeconds;
}

const Clock::time_point& WorldClock::FrameTime() {
	return GetInstance().frameTime;
}

const SysClock::time_point& WorldClock::FrameSysTime() {
	return GetInstance().frameSysTime;
}

const SysClock::time_point& szg::WorldClock::AppLaunchTime() {
	return GetInstance().appLaunchTime;
}

#ifdef DEBUG_FEATURES_ENABLE
#include <imgui.h>
void WorldClock::DebugGui() {
	auto&& instance = GetInstance();
	r32 deltaMS = instance.deltaSeconds * 1000.0f;
	u32 msInteger = static_cast<u32>(deltaMS);
	u32 msDecimal = static_cast<u32>((deltaMS - std::floor(deltaMS)) * 1e4f);
	ImGui::Text(std::format("Delta : {:>5}.{:0>4}ms", msInteger, msDecimal).c_str());
	instance.profiler.debug_gui();
}
#endif // _DEBUG
