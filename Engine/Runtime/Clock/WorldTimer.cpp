#include "WorldTimer.h"

#include "./WorldClock.h"

using namespace szg;

void WorldTimer::ahead(r32 scale) {
	timer += WorldClock::DeltaSeconds() * scale;
}

void WorldTimer::back(r32 scale) {
	timer -= WorldClock::DeltaSeconds() * scale;
}

void WorldTimer::set(r32 time) {
	timer = time;
}

r32 WorldTimer::time() const {
	return timer;
}

bool WorldTimer::just_crossed(r32 time) const noexcept {
	return timer >= time && timer - time <= WorldClock::DeltaSeconds();
}
