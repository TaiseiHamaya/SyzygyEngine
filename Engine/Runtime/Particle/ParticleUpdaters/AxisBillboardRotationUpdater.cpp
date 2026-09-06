#include "AxisBillboardRotationUpdater.h"

#include "Engine/Module/World/Camera/CameraInstance.h"

using namespace szg;

ParticleId AxisBillboardRotationUpdater::register_rotation(const Vector3& axis_, r32 angle_, const Vector3& target_, u32 cameraId_) {
	Payload initial{};
	initial.axis = axis_;
	initial.angle = angle_;
	initial.target = target_;
	initial.cameraId = cameraId_;
	return register_slot(initial);
}

void AxisBillboardRotationUpdater::update(ParticleId id, const Vector3& position, Quaternion& rotation, Reference<const CameraInstance> cameraInstance) {
	Reference<Payload> payload = resolve(id);
	if (!payload) {
		return;
	}
	Vector3 axis = payload->axis.normalize_safe(CVector3::BASIS_Y);
	Vector3 forward;
	if (cameraInstance) {
		Quaternion frontRotation = cameraInstance->world_affine().get_basis().to_quaternion();
		Quaternion flip = CQuaternion::BACK_Y;
		Vector3 cameraForward = CVector3::BASIS_Z * (frontRotation * flip);
		forward = (cameraForward - Vector3::Projection(cameraForward, axis)).normalize_safe();
	}
	else {
		Vector3 target = payload->target;
		Vector3 toTarget = position - target;
		Vector3 lookPosition = target + Vector3::Projection(toTarget, axis);
		forward = (lookPosition - position).normalize_safe();
	}
	if (forward.length() <= 1e-4f) {
		return;
	}
	rotation = Quaternion::AngleAxis(forward, payload->angle) * Quaternion::LookForward(forward, axis);
}
