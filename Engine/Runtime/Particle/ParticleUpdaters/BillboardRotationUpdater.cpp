#include "BillboardRotationUpdater.h"

#include "Engine/Module/World/Camera/CameraInstance.h"
#include "Engine/Runtime/Clock/WorldClock.h"

using namespace szg;

ParticleId BillboardRotationUpdater::register_rotation(r32 angle_, r32 angularVelocity_, const Vector3& target_, u32 cameraId_) {
	Payload initial{};
	initial.angle = angle_;
	initial.angularVelocity = angularVelocity_;
	initial.target = target_;
	initial.cameraId = cameraId_;
	return register_slot(initial);
}

void BillboardRotationUpdater::update(ParticleId id, const Vector3& position, Quaternion& rotation, Reference<const CameraInstance> cameraInstance) {
	Reference<Payload> payload = resolve(id);
	if (!payload) {
		return;
	}
	payload->angle += payload->angularVelocity * WorldClock::DeltaSeconds();

	Quaternion frontRotation;
	if (cameraInstance) {
		frontRotation = cameraInstance->world_affine().get_basis().to_quaternion();
	}
	else {
		Vector3 direction = payload->target - position;
		if (direction.length() <= 1e-4f) {
			return;
		}
		Vector3 forward = direction.normalize_safe();
		frontRotation = Quaternion::LookForward(forward);
	}
	Quaternion flip = CQuaternion::BACK_Y;
	Quaternion spin = Quaternion::AngleAxis(CVector3::BASIS_Z, payload->angle);
	rotation = frontRotation * flip * spin;
}
