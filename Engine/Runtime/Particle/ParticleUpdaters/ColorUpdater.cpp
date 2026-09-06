#include "ColorUpdater.h"

#include <algorithm>

#include <Library/Utility/Tools/Easing.h>
#include <Library/Utility/Tools/MathEPS.h>

using namespace szg;

ParticleId ColorUpdater::register_color(const ColorRGBA& start, const ColorRGBA& end, ParticleEasingMode easing_) {
	Payload initial{};
	initial.current = start;
	initial.keys[0] = Key{ 0.0f, start };
	initial.keys[1] = Key{ 1.0f, end };
	initial.keyCount = 2;
	initial.easing = easing_;
	return register_slot(initial);
}

void ColorUpdater::update(ParticleId id, r32 parametric_) {
	Reference<Payload> payload = resolve(id);
	if (!payload || payload->keyCount == 0) {
		return;
	}
	r32 saturated = eps::saturate(parametric_);
	if (payload->keyCount == 1) {
		payload->current = payload->keys[0].color;
		return;
	}
	u32 nextIndex = 1;
	while (nextIndex + 1 < payload->keyCount && payload->keys[nextIndex].time < saturated) {
		++nextIndex;
	}
	const Key& prev = payload->keys[nextIndex - 1];
	const Key& next = payload->keys[nextIndex];
	r32 interval = next.time - prev.time;
	r32 local = interval <= 0.0f ? 1.0f : eps::saturate((saturated - prev.time) / interval);
	payload->current = ColorRGBA::Lerp(prev.color, next.color, apply_easing(local, payload->easing));
}

void ColorUpdater::set_key(ParticleId id, u32 keyIndex, r32 time_, const ColorRGBA& color_) {
	Reference<Payload> payload = resolve(id);
	if (!payload || keyIndex >= MAX_KEYS) {
		return;
	}
	payload->keys[keyIndex] = Key{ time_, color_ };
	if (keyIndex + 1 > payload->keyCount) {
		payload->keyCount = keyIndex + 1;
	}
	std::sort(payload->keys, payload->keys + payload->keyCount, [](const Key& lhs, const Key& rhs) {
		return lhs.time < rhs.time;
	});
}

r32 ColorUpdater::apply_easing(r32 t, ParticleEasingMode easing_) {
	switch (easing_) {
	case ParticleEasingMode::In:
		return Easing::In::Quad(t);
	case ParticleEasingMode::Out:
		return Easing::Out::Quad(t);
	case ParticleEasingMode::InOut:
		return Easing::InOut::Quad(t);
	case ParticleEasingMode::Linear:
	default:
		return t;
	}
}
