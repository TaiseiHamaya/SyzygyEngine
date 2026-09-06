#include "LifetimeUpdater.h"

#include <Library/Utility/Tools/MathEPS.h>

using namespace szg;

ParticleId LifetimeUpdater::register_lifetime(r32 lifetime_) {
	Payload initial{};
	initial.timer.set(0.0f);
	initial.lifetime = lifetime_;
	return register_slot(initial);
}

void LifetimeUpdater::update(ParticleId id) {
	Reference<Payload> payload = resolve(id);
	if (!payload) {
		return;
	}
	payload->timer.ahead();
}

bool LifetimeUpdater::is_alive(ParticleId id) const {
	Reference<const Payload> payload = resolve(id);
	if (!payload) {
		return false;
	}
	return payload->timer.time() < payload->lifetime;
}

r32 LifetimeUpdater::parametric(ParticleId id) const {
	Reference<const Payload> payload = resolve(id);
	if (!payload || payload->lifetime <= 0.0f) {
		return 1.0f;
	}
	return eps::saturate(payload->timer.time() / payload->lifetime);
}
