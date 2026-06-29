#include "BaseCollider.h"

using namespace szg;

i32 BaseCollider::group_imm() const noexcept {
	return colliderGroupId;
}
