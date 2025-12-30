#include "Particle.h"

using namespace szg;

#include "Engine/Runtime/Clock/WorldClock.h"

#include <Library/Utility/Tools/RandomEngine.h>

Particle::Particle(
	const Vector3& translate,
	r32 lifetime_,
	const Vector3& velocity_, const Vector3& acceleration_,
	const Color4& startColor_, const Color4& endColor_,
	const Vector3& startSize_, const Vector3& endSize_,
	RotationType rotationType_, std::variant<Constant, std::monostate, Random, LookAtAngle> rotationData_
) :
	WorldInstance(),
	lifetime(lifetime_),
	velocity(velocity_), acceleration(acceleration_),
	startColor(startColor_), endColor(endColor_),
	startSize(startSize_), endSize(endSize_),
	rotationType(rotationType_), rotationData(rotationData_) {
	timer.set(0.0f);
	transform.set_translate(translate);
	transform.set_scale(startSize_);
	switch (rotationType) {
	case Particle::RotationType::Constant:
	{
		const auto& data = std::get<Constant>(rotationData);
		transform.set_quaternion(data.value);
		break;
	}
	case Particle::RotationType::Velocity:
		transform.set_quaternion(Quaternion::LookForward(velocity.normalize_safe()));
		break;
	case Particle::RotationType::LookAt:
		look_at(*lookAtDefault);
		break;
	case Particle::RotationType::LookAtAngle:
	{
		const auto& data = std::get<LookAtAngle>(rotationData);
		look_at(*lookAtDefault);
		Vector3 axis = CVector3::FORWARD * transform.get_quaternion();
		transform.set_quaternion(
			Quaternion::AngleAxis(axis, data.angleParSec * timer) * transform.get_quaternion()
		);
		break;
	}
	case Particle::RotationType::Random:
	{
		r32 cos = -2.0f * RandomEngine::Random01MOD() + 1.0f;
		r32 sin = std::sqrt(1.0f - cos * cos);
		r32 phi = PI2 * RandomEngine::Random01MOD();
		Vector3 axis = { sin * std::cos(phi), sin * std::sin(phi), cos };
		r32 angle = PI2 * RandomEngine::Random01MOD();
		transform.set_quaternion(Quaternion::AngleAxis(axis, angle));
		break;
	}
	default:
		break;
	}

	color = startColor_;
}

void Particle::update() {
	timer.ahead();

	r32 parametric = std::min(1.0f, timer / lifetime);

	velocity += acceleration * WorldClock::DeltaSeconds();
	transform.plus_translate(velocity * WorldClock::DeltaSeconds());

	color = Color4::Lerp(startColor, endColor, parametric);

	transform.set_scale(Vector3::Lerp(startSize, endSize, parametric));

	switch (rotationType) {
	case Particle::RotationType::Constant:
	{
		const auto& data = std::get<Constant>(rotationData);
		transform.set_quaternion(data.value);
		break;
	}
	case Particle::RotationType::Velocity:
		transform.set_quaternion(Quaternion::LookForward(velocity.normalize_safe()));
		break;
	case Particle::RotationType::LookAt:
		look_at(*lookAtDefault);
		break;
	case Particle::RotationType::LookAtAngle:
	{
		const auto& data = std::get<LookAtAngle>(rotationData);
		look_at(*lookAtDefault, data.angleParSec * timer * WorldClock::DeltaSeconds());
		break;
	}
	case Particle::RotationType::Random:
	{
		const auto& data = std::get<Random>(rotationData);
		Quaternion amount = Quaternion::AngleAxis(data.axis, data.angle * WorldClock::DeltaSeconds());
		const Quaternion& rotation = transform.get_quaternion();
		transform.set_quaternion(amount * rotation);
		break;
	}
	default:
		break;
	}

	if (timer >= lifetime) {
		isEnded = true;
	}
}

const Matrix4x4 Particle::create_uv_matrix() const {
	return uvTransform.get_matrix4x4_padding();
}
