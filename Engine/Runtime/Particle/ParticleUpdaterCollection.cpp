#include "ParticleUpdaterCollection.h"

#include "Engine/Module/Render/WorldRenderCollection.h"
#include "Engine/Module/World/Camera/CameraInstance.h"
#include "ParticlePool.h"

using namespace szg;

void ParticleUpdaterCollection::setup(Reference<WorldRenderCollection> renderCollection_) {
	renderCollection = renderCollection_;
}

void ParticleUpdaterCollection::clear_all() {
	lifetime.clear();
	pose.clear();
	move.clear();
	drag.clear();
	color.clear();
	scale.clear();
	billboard.clear();
	velocity.clear();
	random.clear();
	axisBillboard.clear();
	uvScroll.clear();
	flipbook.clear();
	alphaFade.clear();
}

void ParticleUpdaterCollection::update_pool(Reference<ParticlePool> pool) {
	if (!pool) {
		return;
	}
	u32 mask = pool->updater_mask();
	ParticleRotationKind rotation = pool->rotation_kind();
	for (u32 slotIndex : pool->alive_indices()) {
		update_particle(pool->instance_at(slotIndex), mask, rotation);
	}
}

void ParticleUpdaterCollection::update_particle(ParticleInstance& instance, u32 mask, ParticleRotationKind rotation) {
	lifetime.update(instance.lifetime);
	pose.update(instance.pose);
	Reference<PoseUpdater::Payload> posePayload = pose.resolve(instance.pose);
	Reference<MoveUpdater::Payload> movePayload = move.resolve(instance.move);
	Reference<ColorUpdater::Payload> colorPayload = color.resolve(instance.color);
	r32 parametric = lifetime.parametric(instance.lifetime);
	if ((mask & static_cast<u32>(ParticleUpdaterKind::DragGravity)) != 0 && movePayload) {
		drag.update(instance.drag, movePayload->velocity);
	}
	if (movePayload && posePayload) {
		move.update(instance.move, posePayload->position);
	}
	if (colorPayload) {
		color.update(instance.color, parametric);
		colorPayload = color.resolve(instance.color);
	}
	if (posePayload) {
		scale.update(instance.scale, parametric, posePayload->scale);
	}
	if (posePayload && movePayload) {
		switch (rotation) {
		case ParticleRotationKind::Billboard:
		{
			Reference<BillboardRotationPayload> billboardPayload = billboard.resolve(instance.rotation);
			Reference<const CameraInstance> cameraInstance = nullptr;
			if (billboardPayload && billboardPayload->cameraId != LOOKAT_INVALID_CAMERA_ID && renderCollection) {
				cameraInstance = renderCollection->camera_instance(billboardPayload->cameraId);
			}
			billboard.update(instance.rotation, posePayload->position, posePayload->rotation, cameraInstance);
			break;
		}
		case ParticleRotationKind::Constant:
			// 初期回転のみ。更新者なし
			break;
		case ParticleRotationKind::VelocityAlign:
			velocity.update(instance.rotation, movePayload->velocity, posePayload->rotation);
			break;
		case ParticleRotationKind::Random:
			random.update(instance.rotation, posePayload->rotation);
			break;
		case ParticleRotationKind::AxisBillboard:
		{
			Reference<AxisBillboardRotationPayload> axisPayload = axisBillboard.resolve(instance.rotation);
			Reference<const CameraInstance> cameraInstance = nullptr;
			if (axisPayload && axisPayload->cameraId != LOOKAT_INVALID_CAMERA_ID && renderCollection) {
				cameraInstance = renderCollection->camera_instance(axisPayload->cameraId);
			}
			axisBillboard.update(instance.rotation, posePayload->position, posePayload->rotation, cameraInstance);
			break;
		}
		}
	}
	if ((mask & static_cast<u32>(ParticleUpdaterKind::UVScroll)) != 0) {
		uvScroll.update(instance.uvScroll);
	}
	if ((mask & static_cast<u32>(ParticleUpdaterKind::Flipbook)) != 0) {
		flipbook.update(instance.flipbook);
	}
	if ((mask & static_cast<u32>(ParticleUpdaterKind::AlphaFade)) != 0 && colorPayload) {
		alphaFade.update(instance.alpha, parametric, colorPayload->current);
	}
}
