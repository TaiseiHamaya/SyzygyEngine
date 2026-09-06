#include "EmitterInstance.h"

#include <Library/Utility/Tools/RandomEngine.h>

#include "./Emission/EmissionDirection.h"
#include "./Emission/EmissionRandom.h"
#include "./Emission/EmissionShape.h"
#include "./ParticlePool.h"

using namespace szg;

EmitterInstance::EmitterInstance() = default;
EmitterInstance::~EmitterInstance() = default;

void EmitterInstance::update() {
	if (!is_active()) {
		return;
	}
	emitTimer.ahead();
	if (!pool) {
		return;
	}
	if (emitTimer.time() < settings.schedule.delay) {
		return;
	}
	if (!settings.schedule.infinite && emittedCycles >= settings.schedule.cycles) {
		return;
	}
	while (emitTimer.time() >= nextEmitTime) {
		if (!settings.schedule.infinite && emittedCycles >= settings.schedule.cycles) {
			break;
		}
		for (u32 i = 0; i < settings.schedule.count; ++i) {
			emit();
		}
		++emittedCycles;
		if (settings.schedule.interval <= 0.0f) {
			break;
		}
		nextEmitTime += settings.schedule.interval;
	}
}

void EmitterInstance::setup_pool(Reference<ParticlePool> pool_) {
	pool = pool_;
}

void EmitterInstance::setup_settings(const EmitterInstanceSettings& settings_) {
	settings = settings_;
	rebuild_emission();
	restart_schedule();
}

void EmitterInstance::restart_schedule() {
	emitTimer.set(0.0f);
	nextEmitTime = settings.schedule.delay;
	emittedCycles = 0;
}

u32 EmitterInstance::BuildUpdaterMask(const EmitterInstanceSettings& settings_) {
	u32 mask = 0;
	mask |= static_cast<u32>(ParticleUpdaterKind::Lifetime);
	mask |= static_cast<u32>(ParticleUpdaterKind::Pose);
	mask |= static_cast<u32>(ParticleUpdaterKind::Move);
	mask |= static_cast<u32>(ParticleUpdaterKind::Color);
	mask |= static_cast<u32>(ParticleUpdaterKind::Scale);
	mask |= static_cast<u32>(ParticleUpdaterKind::Rotation);
	if (settings_.motion.useDragGravity) {
		mask |= static_cast<u32>(ParticleUpdaterKind::DragGravity);
	}
	if (settings_.uv.useUVScroll) {
		mask |= static_cast<u32>(ParticleUpdaterKind::UVScroll);
	}
	if (settings_.flipbook.useFlipbook) {
		mask |= static_cast<u32>(ParticleUpdaterKind::Flipbook);
	}
	if (settings_.fade.useAlphaFade) {
		mask |= static_cast<u32>(ParticleUpdaterKind::AlphaFade);
	}
	return mask;
}

void EmitterInstance::emit() {
	if (!pool || !emissionShape || !emissionDirection) {
		return;
	}
	ParticleSpawnInitial initial{};
	initial.lifetime = ParticleEmissionRandom::Range(settings.initial.lifetimeMin, settings.initial.lifetimeMax);
	Vector3 outward{ CVector3::BASIS_Y };
	Vector3 localPosition{ CVector3::ZERO };
	emissionShape->sample(localPosition, outward);
	if (settings.followParent) {
		initial.position = localPosition * world_affine();
	}
	else {
		initial.position = world_position() + localPosition;
	}
	Vector3 direction = emissionDirection->decide(outward);
	r32 speed = ParticleEmissionRandom::Range(settings.initial.speedMin, settings.initial.speedMax);
	initial.velocity = direction * speed;
	initial.acceleration = ParticleEmissionRandom::Range(settings.initial.accelerationMin, settings.initial.accelerationMax);
	initial.drag = settings.motion.drag;
	initial.gravity = settings.motion.gravity;
	initial.scale = ParticleEmissionRandom::Range(settings.initial.sizeMin, settings.initial.sizeMax);
	initial.scaleEnd = ParticleEmissionRandom::Range(settings.initial.scaleEndMin, settings.initial.scaleEndMax);
	initial.colorStart = ParticleEmissionRandom::Range(settings.initial.colorMin, settings.initial.colorMax);
	initial.colorEnd = ParticleEmissionRandom::Range(settings.initial.colorEndMin, settings.initial.colorEndMax);
	initial.easing = settings.initial.easing;
	if (settings.followParent) {
		initial.rotation = transform_imm().get_quaternion();
	}
	else {
		initial.rotation = CQuaternion::IDENTITY;
	}
	if (settings.rotation.rotationKind == ParticleRotationKind::Constant) {
		initial.rotation = initial.rotation * Quaternion::EulerRadian(settings.rotation.constantRotation);
	}
	if (settings.rotation.rotationKind == ParticleRotationKind::Random) {
		initial.randomAxis = ParticleEmissionRandom::Direction();
		initial.randomAngularVelocity = ParticleEmissionRandom::Range(settings.rotation.randomAngularVelocityMin, settings.rotation.randomAngularVelocityMax);
		initial.rotation = ParticleEmissionRandom::Rotation();
	}
	initial.billboardAngle = ParticleEmissionRandom::Range(settings.rotation.angleMin, settings.rotation.angleMax);
	initial.billboardAngularVelocity = ParticleEmissionRandom::Range(settings.rotation.angularVelocityMin, settings.rotation.angularVelocityMax);
	initial.lookTarget = settings.rotation.lookTarget;
	initial.lookAtCameraId = settings.rotation.lookAtCameraId;
	initial.axisBillboardAxis = settings.rotation.axisBillboardAxis;
	initial.axisBillboardAngle = settings.rotation.axisBillboardAngle;
	initial.uvOffset = CVector2::ZERO;
	initial.uvSpeed = ParticleEmissionRandom::Range(settings.uv.uvSpeedMin, settings.uv.uvSpeedMax);
	initial.flipColumns = settings.flipbook.flipColumns;
	initial.flipRows = settings.flipbook.flipRows;
	initial.flipFrameTime = settings.flipbook.flipFrameTime;
	initial.flipLoop = settings.flipbook.flipLoop;
	initial.fadeIn = settings.fade.fadeIn;
	initial.fadeOut = settings.fade.fadeOut;
	pool->try_spawn(initial);
}

void EmitterInstance::rebuild_emission() {
	emissionShape = ParticleEmissionShape::Create(settings);
	emissionDirection = ParticleEmissionDirection::Create(settings);
}