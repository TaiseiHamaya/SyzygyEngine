#include "EmissionShape.h"

#include <cmath>

#include <Library/Utility/Tools/RandomEngine.h>

#include "../EmitterSettings.h"
#include "./EmissionRandom.h"

using namespace szg;

std::unique_ptr<ParticleEmissionShape> ParticleEmissionShape::Create(const EmitterInstanceSettings& settings_) {
	switch (settings_.emissionShape.shape) {
	case ParticleEmissionShapeKind::Sphere:
		return std::make_unique<SphereEmissionShape>(settings_.emissionShape.sphereRadius);
	case ParticleEmissionShapeKind::Cone:
		return std::make_unique<ConeEmissionShape>(settings_.emissionShape.coneRadius, settings_.emissionShape.coneDirection, settings_.emissionShape.coneAngle);
	case ParticleEmissionShapeKind::Box:
		return std::make_unique<BoxEmissionShape>(settings_.emissionShape.boxSize, settings_.emissionShape.boxOffset);
	case ParticleEmissionShapeKind::Point:
	default:
		return std::make_unique<PointEmissionShape>();
	}
}

void PointEmissionShape::sample(Vector3& localPosition, Vector3& outward) const {
	localPosition = CVector3::ZERO;
	outward = CVector3::BASIS_Y;
}

SphereEmissionShape::SphereEmissionShape(r32 radius_) :
	radius(radius_) {
}

void SphereEmissionShape::sample(Vector3& localPosition, Vector3& outward) const {
	localPosition = ParticleEmissionRandom::Direction() * (radius * std::cbrtf(RandomEngine::Random01MOD()));
	outward = localPosition.normalize_safe();
}

ConeEmissionShape::ConeEmissionShape(r32 radius_, const Vector3& direction_, r32 angle_) :
	radius(radius_), direction(direction_), angle(angle_) {
}

void ConeEmissionShape::sample(Vector3& localPosition, Vector3& outward) const {
	Vector3 axis = direction.normalize_safe();
	Vector3 tangent = (std::fabs(axis.y) < 0.99f ? CVector3::BASIS_Y : CVector3::BASIS_X);
	Vector3 bitangent0 = Vector3::Cross(axis, tangent).normalize_safe();
	Vector3 bitangent1 = Vector3::Cross(axis, bitangent0).normalize_safe();
	r32 discAngle = RandomEngine::Random01MOD() * 2.0f * PI;
	r32 discRadius = radius * RandomEngine::Random01MOD();
	localPosition = bitangent0 * (std::cos(discAngle) * discRadius) + bitangent1 * (std::sin(discAngle) * discRadius);
	outward = axis;
}

BoxEmissionShape::BoxEmissionShape(const Vector3& size_, const Vector3& offset_) :
	size(size_), offset(offset_) {
}

void BoxEmissionShape::sample(Vector3& localPosition, Vector3& outward) const {
	localPosition = offset + Vector3{
		(size.x * (RandomEngine::Random01MOD() * 2.0f - 1.0f)) * 0.5f,
		(size.y * (RandomEngine::Random01MOD() * 2.0f - 1.0f)) * 0.5f,
		(size.z * (RandomEngine::Random01MOD() * 2.0f - 1.0f)) * 0.5f
	};
	outward = (localPosition - offset).normalize_safe(CVector3::BASIS_Y);
}
