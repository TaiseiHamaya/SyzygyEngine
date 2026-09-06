#include "FlipbookUpdater.h"

using namespace szg;

ParticleId FlipbookUpdater::register_flipbook(u32 columns_, u32 rows_, r32 frameTime_, bool isLoop_) {
	Payload initial{};
	initial.columns = columns_ == 0 ? 1 : columns_;
	initial.rows = rows_ == 0 ? 1 : rows_;
	initial.frameTime = frameTime_;
	initial.isLoop = isLoop_;
	initial.timer.set(0.0f);
	initial.index = 0;
	return register_slot(initial);
}

void FlipbookUpdater::update(ParticleId id) {
	Reference<Payload> payload = resolve(id);
	if (!payload || payload->frameTime <= 0.0f) {
		return;
	}
	payload->timer.ahead();
	u32 total = payload->columns * payload->rows;
	u32 frame = static_cast<u32>(payload->timer.time() / payload->frameTime);
	if (payload->isLoop) {
		payload->index = frame % total;
	}
	else if (frame < total) {
		payload->index = frame;
	}
	else {
		payload->index = total - 1;
	}
}
