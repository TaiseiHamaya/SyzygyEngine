#pragma once

#include "../BaseLightInstance.h"

#include <Library/Math/ColorRGB.h>

namespace szg {

struct DirectionalLightData {
	ColorRGB color; // 色
	r32 intensity{ 1 }; // 輝度
	Vector3 direction{ -CVector3::BASIS_Y }; // 向き
};

class DirectionalLightInstance : public BaseLightInstance<DirectionalLightData> {
#ifdef DEBUG_FEATURES_ENABLE
	friend class RemoteDirectionalLightInstance;
#endif // DEBUG_FEATURES_ENABLE
public:
	DirectionalLightInstance() = default;
	virtual ~DirectionalLightInstance() = default;

	DirectionalLightInstance(const DirectionalLightInstance&) = delete;
	DirectionalLightInstance& operator=(const DirectionalLightInstance&&) = delete;
	DirectionalLightInstance(DirectionalLightInstance&&) = default;
	DirectionalLightInstance& operator=(DirectionalLightInstance&&) = default;

public:
};

}; // szg
