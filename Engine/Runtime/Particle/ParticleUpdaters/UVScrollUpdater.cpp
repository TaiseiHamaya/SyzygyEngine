#include "UVScrollUpdater.h"

#include "Engine/Runtime/Clock/WorldClock.h"

using namespace szg;

ParticleId UVScrollUpdater::register_uv(const Vector2& offset_, const Vector2& speed_) {
	Payload initial{};
	initial.offset = offset_;
	initial.speed = speed_;
	return register_slot(initial);
}

void UVScrollUpdater::update(ParticleId id) {
	Reference<Payload> payload = resolve(id);
	if (!payload) {
		return;
	}
	payload->offset += payload->speed * WorldClock::DeltaSeconds();
}
