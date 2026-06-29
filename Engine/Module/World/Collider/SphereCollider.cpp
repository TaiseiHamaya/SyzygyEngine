#include "SphereCollider.h"

using namespace szg;

SphereCollider::SphereCollider(r32 radius_) :
	radius(radius_) {
}

void SphereCollider::set_radius(r32 radius_) {
	radius = radius_;
}

r32 SphereCollider::get_radius() const {
	return radius;
}
