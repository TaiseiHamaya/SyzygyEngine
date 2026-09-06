#include "ScaleUpdater.h"

#include <Library/Utility/Tools/Easing.h>
#include <Library/Utility/Tools/MathEPS.h>

using namespace szg;

ParticleId ScaleUpdater::register_scale(const Vector3& start_, const Vector3& end_, ParticleEasingMode easing_) {
	Payload initial{};
	initial.startScale = start_;
	initial.endScale = end_;
	initial.easing = easing_;
	return register_slot(initial);
}

void ScaleUpdater::update(ParticleId id, r32 parametric_, Vector3& scale) {
	Reference<Payload> payload = resolve(id);
	if (!payload) {
		return;
	}
	r32 t = eps::saturate(parametric_);
	switch (payload->easing) {
	case ParticleEasingMode::In:
		t = Easing::In::Quad(t);
		break;
	case ParticleEasingMode::Out:
		t = Easing::Out::Quad(t);
		break;
	case ParticleEasingMode::InOut:
		t = Easing::InOut::Quad(t);
		break;
	case ParticleEasingMode::Linear:
	default:
		break;
	}
	scale = Vector3::Lerp(payload->startScale, payload->endScale, t);
}
