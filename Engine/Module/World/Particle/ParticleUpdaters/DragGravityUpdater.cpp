#include "DragGravityUpdater.h"

#include <Library/Utility/Tools/MathEPS.h>

#include "Engine/Runtime/Clock/WorldClock.h"

using namespace szg;

ParticleId DragGravityUpdater::register_drag_gravity(r32 drag_, const Vector3& gravity_) {
	Payload initial{};
	initial.drag = drag_;
	initial.gravity = gravity_;
	return register_slot(initial);
}

void DragGravityUpdater::update(ParticleId id, Vector3& velocity) {
	Reference<Payload> payload = resolve(id);
	if (!payload) {
		return;
	}
	r32 deltaTime = WorldClock::DeltaSeconds();
	velocity *= 1.0f - eps::saturate(payload->drag * deltaTime);
	velocity += payload->gravity * deltaTime;
}
