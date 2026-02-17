#pragma once

#include <unordered_map>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

class Affine;

namespace szg {

class WorldInstance;

};

class Hierarchy final {
public:
	Hierarchy(Reference<const Affine> selfAffine_);
	~Hierarchy() = default;

	SZG_CLASS_MOVE_ONLY(Hierarchy)

public:
	void set_parent(Reference<szg::WorldInstance> instance) noexcept;

	bool has_parent() const noexcept;

	void reset_parent() noexcept;

	void add_child(Reference<szg::WorldInstance> instance);

	void remove_child(Reference<szg::WorldInstance> instance) noexcept;

	Reference<const szg::WorldInstance> parent_imm() const noexcept;
	Reference<szg::WorldInstance> parent_mut() noexcept;

	const std::unordered_map<u64, Reference<szg::WorldInstance>>& children_imm() const noexcept;
	std::unordered_map<u64, Reference<szg::WorldInstance>>& children_mut() noexcept;

	const Affine& parent_affine() const;
	const Affine& parent_affine_safe() const noexcept;


private:
	Reference<szg::WorldInstance> parent;
	std::unordered_map<u64, Reference<szg::WorldInstance>> children;

	Reference<const Affine> selfAffine;
};
