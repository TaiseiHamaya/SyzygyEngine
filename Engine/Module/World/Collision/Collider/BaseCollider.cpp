#include "BaseCollider.h"

using namespace szg;

BaseCollider::BaseCollider() : 
	WorldInstance() {
	groupName = nullptr;
}

const std::string& BaseCollider::group() const noexcept {
	if (groupName) {
		return *groupName;
	}
	else {
		static std::string ColliderZeroString{};
		return ColliderZeroString;
	}
}

void BaseCollider::set_group_name(const std::string& name) {
	groupName = &name;
}

#ifdef DEBUG_FEATURES_ENABLE

Matrix4x4 BaseCollider::debug_matrix() const {
	return debugMatrix * Transform3D::MakeTranslateMatrix(world_position());
}

#endif // _DEBUG
