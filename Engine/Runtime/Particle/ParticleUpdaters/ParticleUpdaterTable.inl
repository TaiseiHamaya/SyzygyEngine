#pragma once

#include "ParticleUpdater.h"

template<typename Payload>
szg::ParticleId szg::ParticleUpdaterTable<Payload>::register_slot(const Payload& initial) {
	u32 slotIndex = 0;
	if (!freeList.empty()) {
		slotIndex = freeList.back();
		freeList.pop_back();
		Slot& slot = slots[slotIndex];
		slot.alive = true;
		slot.payload = initial;
	}
	else {
		slotIndex = static_cast<u32>(slots.size());
		Slot& slot = slots.emplace_back();
		slot.alive = true;
		slot.payload = initial;
	}
	++aliveCount;
	return ParticleId{ slotIndex, slots[slotIndex].generation };
}

template<typename Payload>
void szg::ParticleUpdaterTable<Payload>::unregister(szg::ParticleId id) {
	if (!is_current(id)) {
		return;
	}
	Slot& slot = slots[id.index];
	slot.alive = false;
	++slot.generation;
	--aliveCount;
	freeList.emplace_back(id.index);
}

template<typename Payload>
Reference<Payload> szg::ParticleUpdaterTable<Payload>::resolve(szg::ParticleId id) {
	if (!is_current(id)) {
		return nullptr;
	}
	return slots[id.index].payload;
}

template<typename Payload>
Reference<const Payload> szg::ParticleUpdaterTable<Payload>::resolve(szg::ParticleId id) const {
	if (!is_current(id)) {
		return nullptr;
	}
	return slots[id.index].payload;
}

template<typename Payload>
void szg::ParticleUpdaterTable<Payload>::clear() {
	slots.clear();
	freeList.clear();
	aliveCount = 0;
}

template<typename Payload>
u32 szg::ParticleUpdaterTable<Payload>::alive_count() const {
	return aliveCount;
}

template<typename Payload>
void szg::ParticleUpdaterTable<Payload>::reserve(u32 capacity) {
	slots.reserve(capacity);
	freeList.reserve(capacity);
}

template<typename Payload>
bool szg::ParticleUpdaterTable<Payload>::is_current(szg::ParticleId id) const {
	if (!id.is_valid() || id.index >= slots.size()) {
		return false;
	}
	const Slot& slot = slots[id.index];
	return slot.alive && slot.generation == id.generation;
}
