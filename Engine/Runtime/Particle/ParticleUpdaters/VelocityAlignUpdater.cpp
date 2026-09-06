#include "VelocityAlignUpdater.h"

using namespace szg;

ParticleId VelocityAlignUpdater::register_rotation() {
	return register_slot(Payload{});
}

void VelocityAlignUpdater::update(ParticleId id, const Vector3& velocity, Quaternion& rotation) {
	if (!resolve(id)) {
		return;
	}
	if (velocity.length() <= 1e-4f) {
		return;
	}
	rotation = Quaternion::LookForward(velocity.normalize_safe());
}
