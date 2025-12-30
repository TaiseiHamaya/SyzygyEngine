#include "WorldInstance.h"

using namespace szg;

#include "Engine/Module/Manager/World/WorldRoot.h"

void WorldInstance::update_affine() {
	if (!isActive) {
		return;
	}
	fixed_update();
	affine = create_world_affine();

	for (auto& [_, child] : hierarchy.children_mut()) {
		child->update_affine();
	}
}

Affine WorldInstance::create_world_affine() const {
	Affine result = Affine::FromTransform3D(transform);
	if (hierarchy.has_parent()) {
		result *= hierarchy.parent_affine();
	}
	return result;
}

void WorldInstance::look_at(Reference<const WorldInstance> rhs, r32 angle, const Vector3& upward) noexcept {
	if (!rhs) {
		return;
	}
	look_at(rhs->world_position(), angle, upward);
}

// 既知の不具合 : 特定環境でlook_atが正しくならない場合がある
void WorldInstance::look_at(const Vector3& point, r32 angle, const Vector3& upward) noexcept {
	Vector3 localForward;
	Vector3 localUpward;
	if (hierarchy.has_parent()) {
		Affine parentInversedWorldAffine = hierarchy.parent_affine().inverse_fast();
		Vector3 rhsObjectCoordinatePosition = point * parentInversedWorldAffine;
		localUpward = upward * parentInversedWorldAffine.get_basis();
		localForward = (rhsObjectCoordinatePosition - transform.get_translate()).normalize_safe();
	}
	else {
		localUpward = upward;
		localForward = (point - transform.get_translate()).normalize_safe();
	}
	transform.set_quaternion(Quaternion::AngleAxis(localForward, angle) * Quaternion::LookForward(localForward, localUpward));
}

void WorldInstance::look_at_axis(Reference<const WorldInstance> target, r32 angle, const Vector3& axis) noexcept {
	if (!target) {
		return;
	}
	look_at_axis(target->world_position(), angle, axis);
}

void WorldInstance::look_at_axis(const Vector3& point, r32 angle, const Vector3& axis) noexcept {
	Vector3 toTarget = world_position() - point;
	Vector3 lookPosition = point + Vector3::Projection(toTarget, axis);
	look_at(lookPosition, angle, axis);
}

Reference<const WorldInstance> WorldInstance::parent_imm() const noexcept {
	return hierarchy.parent_imm();
}

Reference<WorldInstance> WorldInstance::parent_mut() noexcept {
	return hierarchy.parent_mut();
}

void WorldInstance::reparent(Reference<WorldInstance> parent, bool isKeepPose) {
	// 古い親から削除
	if (hierarchy.has_parent()) {
		hierarchy.parent_mut()->detach_child(this);
	}
	// transformの計算
	if (isKeepPose) {
		const Affine& worldAffine = world_affine();
		if (parent) {
			const Affine parentAffineInv = parent->world_affine().inverse();
			const Affine local = worldAffine * parentAffineInv;
			const Basis& basis = local.get_basis();
			transform.set_scale(basis.to_scale());
			transform.set_quaternion(basis.to_quaternion());
			transform.set_translate(local.get_origin());
		}
		else {
			const Basis& basis = worldAffine.get_basis();
			transform.set_scale(basis.to_scale());
			transform.set_quaternion(basis.to_quaternion());
			transform.set_translate(worldAffine.get_origin());
		}
	}
	// 親の登録
	hierarchy.set_parent(parent);
	// 新親に登録
	if (parent) {
		parent->attach_child(this);
	}
	// 深度の再計算
	recalculate_depth();
}

void szg::WorldInstance::destroy_self() {
	worldRoot->destroy(this);
}

void WorldInstance::mark_destroy() {
	isDestroy = true;
	on_mark_destroy();
	// 子も削除予定にする
	for (auto& [_, child] : hierarchy.children_mut()) {
		child->destroy_self();
	}
}

void WorldInstance::setup_id(u64 id) {
	instanceId = id;
}

u64 WorldInstance::instance_id() const {
	return instanceId;
}

void WorldInstance::setup_world_root(Reference<WorldRoot> worldRoot_) {
	worldRoot = worldRoot_;
}

Reference<WorldRoot> WorldInstance::world_root_mut() const {
	return worldRoot;
}

void WorldInstance::detach_child(Reference<WorldInstance> child) {
	hierarchy.remove_child(child);
}

void WorldInstance::attach_child(Reference<WorldInstance> child) {
	hierarchy.add_child(child);
}

void WorldInstance::recalculate_depth() {
	if (hierarchy.has_parent()) {
		hierarchyDepth = hierarchy.parent_imm()->hierarchy_depth() + 1;
	}
	else {
		hierarchyDepth = 0;
	}
	// 子の深度も再計算
	for (auto& [_, child] : hierarchy.children_mut()) {
		child->recalculate_depth();
	}
}
