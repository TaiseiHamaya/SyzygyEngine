#pragma once

#include "../BaseLightInstance.h"

#include <Library/Math/ColorRGB.h>

namespace szg {

struct SpotLightData {
	ColorRGB color; // 色
	r32 intensity{ 1 }; // 輝度

	Vector3 position; // 位置
	r32 distance; // ライト距離
	
	Vector3 direction{ CVector3::DOWN }; // 向き
	r32 decay; // 距離減衰率

	r32 angle; // ライト範囲
	r32 falloffStart; // ライト
};

class SpotLightInstance : public BaseLightInstance<SpotLightData> {
public:
	SpotLightInstance() = default;
	virtual ~SpotLightInstance() = default;

	SpotLightInstance(const SpotLightInstance&) = delete;
	SpotLightInstance& operator=(const SpotLightInstance&&) = delete;
	SpotLightInstance(SpotLightInstance&&) = default;
	SpotLightInstance& operator=(SpotLightInstance&&) = default;

public:
};


}; // szg
