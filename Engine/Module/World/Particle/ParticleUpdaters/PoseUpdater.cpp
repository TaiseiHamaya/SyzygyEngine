#include "PoseUpdater.h"

using namespace szg;

ParticleId PoseUpdater::register_pose(const Vector3& position_, const Quaternion& rotation_, const Vector3& scale_) {
	Payload initial{};
	initial.position = position_;
	initial.rotation = rotation_;
	initial.scale = scale_;
	return register_slot(initial);
}

void PoseUpdater::update(ParticleId id) {
	Reference<Payload> payload = resolve(id);
	if (!payload) {
		return;
	}
	payload->rotation = payload->rotation.normalize();
}
