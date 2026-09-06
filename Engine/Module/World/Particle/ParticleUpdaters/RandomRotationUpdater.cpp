#include "RandomRotationUpdater.h"

#include "Engine/Runtime/Clock/WorldClock.h"

using namespace szg;

ParticleId RandomRotationUpdater::register_rotation(const Vector3& axis_, r32 angularVelocity_) {
	Payload initial{};
	initial.axis = axis_;
	initial.angularVelocity = angularVelocity_;
	return register_slot(initial);
}

void RandomRotationUpdater::update(ParticleId id, Quaternion& rotation) {
	Reference<Payload> payload = resolve(id);
	if (!payload) {
		return;
	}
	rotation = Quaternion::AngleAxis(payload->axis, payload->angularVelocity * WorldClock::DeltaSeconds()) * rotation;
}
