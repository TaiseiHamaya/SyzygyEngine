#include "Hierarchy.h"

using namespace szg;

#include "Affine.h"

#include "Engine/Module/World/WorldInstance/WorldInstance.h"

Hierarchy::Hierarchy(Reference<const Affine> selfAffine_) {
	selfAffine = selfAffine_;
	parent = nullptr;
}

void Hierarchy::set_parent(Reference<WorldInstance> instance) noexcept {
	parent = instance;
}

void Hierarchy::reset_parent() noexcept {
	parent = nullptr;
}

Reference<const WorldInstance> Hierarchy::parent_imm() const noexcept {
	return parent;
}

Reference<WorldInstance> Hierarchy::parent_mut() noexcept {
	return parent;
}

bool Hierarchy::has_parent() const noexcept {
	return parent;
}

const Affine& Hierarchy::parent_affine() const {
	return parent->world_affine();
}

const Affine& Hierarchy::parent_affine_safe() const noexcept {
	if (has_parent()) {
		return parent_affine();
	}
	return CAffine::IDENTITY;
}

void Hierarchy::add_child(Reference<WorldInstance> instance) {
	if (instance.is_null()) {
		return;
	}
	children.emplace(instance->instance_id(), instance);
}

void Hierarchy::remove_child(Reference<WorldInstance> instance) noexcept {
	children.erase(instance->instance_id());
}

const std::unordered_map<u64, Reference<WorldInstance>>& Hierarchy::children_imm() const noexcept {
	return children;
}

std::unordered_map<u64, Reference<WorldInstance>>& Hierarchy::children_mut() noexcept {
	return children;
}
