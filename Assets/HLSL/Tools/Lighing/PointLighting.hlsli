#ifndef POINT_LIGHTING_HLSLI
#define POINT_LIGHTING_HLSLI

#include "../Pixel.hlsli"
#include "CalcLighting.hlsli"

struct PointLightBuffer {
	float3 color; // 色
	float intensity; // 輝度
	float3 position; // 位置
	float radius; // 範囲
	float decay; // 減衰率
};

LightingData CalcLightingData(Pixel pixel, float3 camera, PointLightBuffer light) {
	// specular
	float3 toCamera = normalize(camera - pixel.world);
	float3 direction = normalize(pixel.world - light.position);
	float distance = length(pixel.world - light.position);
	
	float factor = pow(saturate(-distance / light.radius + 1.0f), light.decay);
	if (isnan(factor)) {
		factor = 0;
	}
	
	LightingData data;
	data.incoming.radiance = light.color.rgb * light.intensity * factor;
	data.incoming.direction = normalize(pixel.world - light.position);
	data.surface.albedo = pixel.color;
	data.surface.normal = pixel.normal;
	data.surface.shininess = pixel.shininess;
	data.surface.toCamera = normalize(camera - pixel.world);
	
	return data;
};

#endif // !POINT_LIGHTING_HLSLI
