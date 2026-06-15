#pragma once

#include "BaseCollider.h"

namespace szg {

class SphereCollider : public BaseCollider {
public:
	SphereCollider() = default;
	SphereCollider(r32 radius_);
	~SphereCollider() = default;

public:
	void set_radius(r32 radius_);
	r32 get_radius() const;

private:
	r32 radius{ 1 };
};

}; // szg
