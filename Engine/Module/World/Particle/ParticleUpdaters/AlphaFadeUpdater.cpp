#include "AlphaFadeUpdater.h"

#include <Library/Utility/Tools/Functions.h>
#include <Library/Utility/Tools/MathEPS.h>

using namespace szg;

ParticleId AlphaFadeUpdater::register_fade(r32 fadeIn_, r32 fadeOut_) {
	Payload initial{};
	initial.fadeIn = fadeIn_;
	initial.fadeOut = fadeOut_;
	return register_slot(initial);
}

void AlphaFadeUpdater::update(ParticleId id, r32 parametric_, ColorRGBA& color) {
	Reference<Payload> payload = resolve(id);
	if (!payload) {
		return;
	}
	r32 saturated = eps::saturate(parametric_);
	r32 fade = 1.0f;
	if (payload->fadeIn > 0.0f) {
		fade *= eps::smoothstep(0.0f, payload->fadeIn, saturated);
	}
	if (payload->fadeOut > 0.0f) {
		fade *= 1.0f - eps::smoothstep(1.0f - payload->fadeOut, 1.0f, saturated);
	}
	color.alpha *= eps::saturate(fade);
}
