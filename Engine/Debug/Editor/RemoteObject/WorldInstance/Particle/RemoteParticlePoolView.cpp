#ifdef DEBUG_FEATURES_ENABLE

#include "RemoteParticlePoolView.h"

#include <imgui.h>

#include "Engine/Module/World/Particle/ParticlePool.h"

using namespace szg;

void RemoteParticlePoolView::refresh(Reference<ParticlePool> pool) {
	if (!pool) {
		aliveCount = 0;
		maxCount = 0;
		overflowPolicyName = "Unknown";
		return;
	}
	aliveCount = pool->alive_count();
	maxCount = pool->max();
	overflowPolicyName = pool->overflow_policy() == ParticleOverflowPolicy::ReuseOldest ? "ReuseOldest" : "Discard";
}

void RemoteParticlePoolView::draw_inspector() const {
	ImGui::Text("Alive : %u / %u", aliveCount, maxCount);
	ImGui::Text("Overflow : %s", overflowPolicyName.c_str());
}

#endif // DEBUG_FEATURES_ENABLE
