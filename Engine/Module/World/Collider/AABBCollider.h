#pragma once

#include "BaseCollider.h"

namespace szg {

class AABBCollider : public BaseCollider {
public:
	AABBCollider() = default;
	AABBCollider(const Vector3& size_, const Vector3& offset_ = CVector3::ZERO);
	~AABBCollider() = default;

public:
	const Vector3& min_vertex() const;
	const Vector3& max_vertex() const;

private:
	Vector3 min{ CVector3::ZERO };
	Vector3 max{ CVector3::BASIS };
};

}; // szg
