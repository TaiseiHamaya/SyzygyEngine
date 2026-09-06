#include "ParticlePool.h"

#include "./ParticleUpdaterCollection.h"
#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"
#include "Engine/Assets/Texture/TextureAsset.h"
#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/Module/DrawExecutor/Particle/ParticleBillboardDrawManager.h"
#include "Engine/Module/DrawExecutor/Particle/ParticleDrawItem.h"
#include "Engine/Module/DrawExecutor/Particle/ParticleMeshDrawManager.h"
#include "Engine/Module/World/Particle/EmitterInstance.h"

using namespace szg;

ParticlePool::~ParticlePool() {
	clear();
}

void ParticlePool::setup(Reference<ParticleUpdaterCollection> updaters_, Reference<EmitterInstance> owner_, u32 capacity_, ParticleOverflowPolicy policy_) {
	updaters = updaters_;
	owner = owner_;
	capacity = capacity_;
	overflowPolicy = policy_;
	slots.resize(capacity);
	freeList.reserve(capacity);
	aliveIndices.reserve(capacity);
	for (u32 i = 0; i < capacity; ++i) {
		freeList.emplace_back(capacity - 1 - i);
	}
	aliveCount = 0;
	reuseCursor = 0;
}

void ParticlePool::setup_draw_spec(const ParticleDrawSpec& spec_) {
	drawSpec = spec_;
	texture = nullptr;
	mesh = nullptr;
	if (TextureLibrary::IsRegistered(drawSpec.textureName)) {
		texture = TextureLibrary::GetTexture(drawSpec.textureName);
	}
	if (!texture) {
		texture = TextureLibrary::GetTexture("Error.png");
	}
	if (drawSpec.drawType == ParticleDrawType::Mesh) {
		std::string meshName = drawSpec.meshName;
		if (!PolygonMeshLibrary::IsRegistered(meshName)) {
			meshName = "ErrorObject.obj";
		}
		mesh = PolygonMeshLibrary::GetPolygonMesh(meshName);
	}
}

void ParticlePool::setup_updaters(u32 mask_, ParticleRotationKind rotation_) {
	updaterMask = mask_;
	rotationKind = rotation_;
}

bool ParticlePool::try_spawn(const ParticleSpawnInitial& initial) {
	if (!updaters) {
		return false;
	}
	u32 slotIndex = 0;
	if (!freeList.empty()) {
		slotIndex = freeList.back();
		freeList.pop_back();
	}
	else {
		if (overflowPolicy == ParticleOverflowPolicy::Discard || aliveCount == 0) {
			return false;
		}
		slotIndex = aliveIndices[reuseCursor % aliveCount];
		reuseCursor++;
		unregister_slot(slotIndex);
		remove_alive(slotIndex);
	}
	Slot& slot = slots[slotIndex];
	ParticleUpdaterCollection& collection = *updaters;
	slot.ids.lifetime = collection.lifetime_mut().register_lifetime(initial.lifetime);
	slot.ids.pose = collection.pose_mut().register_pose(initial.position, initial.rotation, initial.scale);
	slot.ids.move = collection.move_mut().register_move(initial.velocity, initial.acceleration);
	slot.ids.drag = collection.drag_mut().register_drag_gravity(initial.drag, initial.gravity);
	slot.ids.color = collection.color_mut().register_color(initial.colorStart, initial.colorEnd, initial.easing);
	slot.ids.scale = collection.scale_mut().register_scale(initial.scale, initial.scaleEnd, initial.easing);
	switch (rotationKind) {
	case ParticleRotationKind::Billboard:
		slot.ids.rotation = collection.billboard_mut().register_rotation(initial.billboardAngle, initial.billboardAngularVelocity, initial.lookTarget, initial.lookAtCameraId);
		break;
	case ParticleRotationKind::Constant:
		// 初期回転のみ。更新者なし
		break;
	case ParticleRotationKind::VelocityAlign:
		slot.ids.rotation = collection.velocity_mut().register_rotation();
		break;
	case ParticleRotationKind::Random:
		slot.ids.rotation = collection.random_mut().register_rotation(initial.randomAxis, initial.randomAngularVelocity);
		break;
	case ParticleRotationKind::AxisBillboard:
		slot.ids.rotation = collection.axis_billboard_mut().register_rotation(initial.axisBillboardAxis, initial.axisBillboardAngle, initial.lookTarget, initial.lookAtCameraId);
		break;
	}
	slot.ids.uvScroll = collection.uv_mut().register_uv(initial.uvOffset, initial.uvSpeed);
	slot.ids.flipbook = collection.flip_mut().register_flipbook(initial.flipColumns, initial.flipRows, initial.flipFrameTime, initial.flipLoop);
	slot.ids.alpha = collection.alpha_mut().register_fade(initial.fadeIn, initial.fadeOut);
	slot.alive = true;
	slot.alivePosition = aliveCount;
	aliveIndices.emplace_back(slotIndex);
	++aliveCount;
	return true;
}

void ParticlePool::reclaim() {
	if (!updaters) {
		return;
	}
	for (u32 i = 0; i < aliveIndices.size();) {
		u32 slotIndex = aliveIndices[i];
		const Slot& slot = slots[slotIndex];
		if (updaters->lifetime_mut().is_alive(slot.ids.lifetime)) {
			++i;
			continue;
		}
		unregister_slot(slotIndex);
		remove_alive(slotIndex);
	}
}

void ParticlePool::clear() {
	if (updaters) {
		for (u32 slotIndex : aliveIndices) {
			unregister_slot(slotIndex);
			slots[slotIndex].alive = false;
		}
	}
	aliveIndices.clear();
	freeList.clear();
	for (u32 i = 0; i < capacity; ++i) {
		freeList.emplace_back(capacity - 1 - i);
		slots[i].ids.reset();
		slots[i].alive = false;
	}
	aliveCount = 0;
	reuseCursor = 0;
}

bool ParticlePool::is_owner_destroyed() const {
	if (!owner) {
		return true;
	}
	return owner->is_marked_destroy();
}

void ParticlePool::sync_draw(ParticleBillboardDrawManager& billboardManager, ParticleMeshDrawManager& meshManager) {
	if (!updaters || aliveCount == 0) {
		return;
	}
	bool isBillboard = drawSpec.drawType == ParticleDrawType::Billboard;
	if (isBillboard) {
		billboardManager.ensure_instancing(drawSpec.layer, drawSpec.blend, capacity);
	}
	else {
		meshManager.ensure_instancing(drawSpec.layer, drawSpec.meshName, drawSpec.blend, capacity, mesh);
	}
	u32 textureIndex = TextureLibrary::GetTexture("Error.png")->index();
	if (texture) {
		textureIndex = texture->index();
	}
	for (u32 slotIndex : aliveIndices) {
		const Slot& slot = slots[slotIndex];
		Reference<const PoseUpdater::Payload> pose = updaters->pose_imm().resolve(slot.ids.pose);
		Reference<const ColorUpdater::Payload> color = updaters->color_imm().resolve(slot.ids.color);
		if (!pose || !color) {
			continue;
		}
		ParticleDrawItem item{};
		item.enabled = true;
		item.renderLayer = drawSpec.layer;
		item.blend = drawSpec.blend;
		item.meshName = drawSpec.meshName;
		item.world = Affine::FromSRT(pose->scale, pose->rotation, pose->position);
		item.size = Vector2{ pose->scale.x, pose->scale.y };
		item.pivot = drawSpec.pivot;
		item.color = color->current;
		item.textureIndex = textureIndex;
		if (isBillboard && rotationKind == ParticleRotationKind::Billboard) {
			Reference<const BillboardRotationUpdater::Payload> rotation = updaters->billboard_imm().resolve(slot.ids.rotation);
			if (rotation) {
				item.billboardAngle = rotation->angle;
			}
		}
		if ((updaterMask & static_cast<u32>(ParticleUpdaterKind::UVScroll)) != 0
			|| (updaterMask & static_cast<u32>(ParticleUpdaterKind::Flipbook)) != 0) {
			Matrix3x3 uv = CMatrix3x3::IDENTITY;
			if ((updaterMask & static_cast<u32>(ParticleUpdaterKind::Flipbook)) != 0) {
				Reference<const FlipbookUpdater::Payload> flip = updaters->flip_imm().resolve(slot.ids.flipbook);
				if (flip && flip->columns > 0 && flip->rows > 0) {
					u32 column = flip->index % flip->columns;
					u32 row = flip->index / flip->columns;
					Vector2 cellSize{ 1.0f / flip->columns, 1.0f / flip->rows };
					Vector2 cellOffset{ cellSize.x * column, cellSize.y * row };
					uv = Transform2D{ cellSize, 0.0f, cellOffset }.get_matrix() * uv;
				}
			}
			if ((updaterMask & static_cast<u32>(ParticleUpdaterKind::UVScroll)) != 0) {
				Reference<const UVScrollUpdater::Payload> scroll = updaters->uv_imm().resolve(slot.ids.uvScroll);
				if (scroll) {
					uv = Transform2D{ CVector2::BASIS, 0.0f, scroll->offset }.get_matrix() * uv;
				}
			}
			item.uvTransform = uv;
		}
		if (isBillboard) {
			billboardManager.transfer_item(item);
		}
		else {
			meshManager.transfer_item(item);
		}
	}
}

void ParticlePool::unregister_slot(u32 slotIndex) {
	Slot& slot = slots[slotIndex];
	ParticleUpdaterCollection& collection = *updaters;
	collection.lifetime_mut().unregister(slot.ids.lifetime);
	collection.pose_mut().unregister(slot.ids.pose);
	collection.move_mut().unregister(slot.ids.move);
	collection.drag_mut().unregister(slot.ids.drag);
	collection.color_mut().unregister(slot.ids.color);
	collection.scale_mut().unregister(slot.ids.scale);
	switch (rotationKind) {
	case ParticleRotationKind::Billboard:
		collection.billboard_mut().unregister(slot.ids.rotation);
		break;
	case ParticleRotationKind::Constant:
		// 初期回転のみ。更新者なし
		break;
	case ParticleRotationKind::VelocityAlign:
		collection.velocity_mut().unregister(slot.ids.rotation);
		break;
	case ParticleRotationKind::Random:
		collection.random_mut().unregister(slot.ids.rotation);
		break;
	case ParticleRotationKind::AxisBillboard:
		collection.axis_billboard_mut().unregister(slot.ids.rotation);
		break;
	}
	collection.uv_mut().unregister(slot.ids.uvScroll);
	collection.flip_mut().unregister(slot.ids.flipbook);
	collection.alpha_mut().unregister(slot.ids.alpha);
	slot.ids.reset();
}

void ParticlePool::remove_alive(u32 slotIndex) {
	Slot& slot = slots[slotIndex];
	u32 position = slot.alivePosition;
	u32 lastIndex = aliveIndices.back();
	aliveIndices[position] = lastIndex;
	slots[lastIndex].alivePosition = position;
	aliveIndices.pop_back();
	slot.alive = false;
	--aliveCount;
	freeList.emplace_back(slotIndex);
}
