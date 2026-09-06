#include "MoveUpdater.h"

#include "Engine/Runtime/Clock/WorldClock.h"

using namespace szg;

ParticleId MoveUpdater::register_move(const Vector3& velocity_, const Vector3& acceleration_) {
	Payload initial{};
	initial.velocity = velocity_;
	initial.acceleration = acceleration_;
	return register_slot(initial);
}

void MoveUpdater::update(ParticleId id, Vector3& position) {
	Reference<Payload> payload = resolve(id);
	if (!payload) {
		return;
	}
	r32 deltaTime = WorldClock::DeltaSeconds();
	payload->velocity += payload->acceleration * deltaTime;
	position += payload->velocity * deltaTime;
}
