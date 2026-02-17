#include "SphereCollider.h"

using namespace szg;

SphereCollider::SphereCollider(r32 radius_) :
	radius(radius_) {

#ifdef DEBUG_FEATURES_ENABLE
	debugMatrix = Transform3D::MakeScaleMatrix({ radius, radius , radius });
#endif // _DEBUG
}

void SphereCollider::set_radius(r32 radius_) {
	radius = radius_;

#ifdef DEBUG_FEATURES_ENABLE
	debugMatrix = Transform3D::MakeScaleMatrix({ radius, radius , radius });
#endif // _DEBUG
}

r32 SphereCollider::get_radius() const {
	return radius;
}
