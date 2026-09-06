#include "ParticleInstance.h"

using namespace szg;

void ParticleInstance::reset() {
	lifetime = ParticleId{};
	pose = ParticleId{};
	move = ParticleId{};
	drag = ParticleId{};
	color = ParticleId{};
	scale = ParticleId{};
	rotation = ParticleId{};
	uvScroll = ParticleId{};
	flipbook = ParticleId{};
	alpha = ParticleId{};
}
