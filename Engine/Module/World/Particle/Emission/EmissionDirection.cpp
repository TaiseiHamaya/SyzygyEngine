#include "EmissionDirection.h"

#include <cmath>

#include <Library/Utility/Tools/RandomEngine.h>

#include "../EmitterSettings.h"

using namespace szg;

std::unique_ptr<ParticleEmissionDirection> ParticleEmissionDirection::Create(const EmitterInstanceSettings& settings_) {
	switch (settings_.emissionDirection.directionMode) {
	case ParticleDirectionMode::EmissionShape:
		return std::make_unique<ShapeEmissionDirection>();
	case ParticleDirectionMode::AngleRange:
		return std::make_unique<AngleRangeEmissionDirection>(settings_.emissionDirection.angleBaseDirection, settings_.emissionDirection.angleRange);
	case ParticleDirectionMode::Constant:
	default:
		return std::make_unique<ConstantEmissionDirection>(settings_.emissionDirection.constantDirection);
	}
}

ConstantEmissionDirection::ConstantEmissionDirection(const Vector3& direction_) :
	direction(direction_) {
}

Vector3 ConstantEmissionDirection::decide(const Vector3& outward) const {
	static_cast<void>(outward);
	return direction.normalize_safe();
}

Vector3 ShapeEmissionDirection::decide(const Vector3& outward) const {
	return outward.normalize_safe();
}

AngleRangeEmissionDirection::AngleRangeEmissionDirection(const Vector3& baseDirection_, r32 angle_) :
	baseDirection(baseDirection_), angle(angle_) {
}

Vector3 AngleRangeEmissionDirection::decide(const Vector3& outward) const {
	static_cast<void>(outward);
	Vector3 base = baseDirection.normalize_safe();
	Vector3 tangent = (fabsf(base.y) < 0.99f ? CVector3::BASIS_Y : CVector3::BASIS_X);
	Vector3 bitangent0 = Vector3::Cross(base, tangent).normalize_safe();
	Vector3 bitangent1 = Vector3::Cross(base, bitangent0).normalize_safe();
	r32 discAngle = RandomEngine::Random01MOD() * 2.0f * PI;
	r32 tilt = angle * RandomEngine::Random01MOD();
	return (base + (bitangent0 * cosf(discAngle) + bitangent1 * sinf(discAngle)) * tanf(tilt)).normalize_safe();
}
