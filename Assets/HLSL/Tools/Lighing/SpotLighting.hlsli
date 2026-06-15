#ifndef SPOT_LIGHTING_HLSLI
#define SPOT_LIGHTING_HLSLI

#include "CalcLighting.hlsli"

struct SpotLight {
	float3 color;       // 色
	float intensity;    // 輝度
	float3 position;    // 位置
	float decay;        // 距離減衰率
	float3 direction;   // 向き
	float distance;     // ライト距離
	float angle;        // ライト範囲
	float falloffStart; //
};

LightingData CalcLightingData(Pixel pixel, float3 camera, SpotLight light) {
	// specular
	float3 toCamera = normalize(camera - pixel.world);
	float3 direction = normalize(pixel.world - light.position);
	// 距離減衰
	float distance = length(pixel.world - light.position);
	float distanceDecayFactor = pow(saturate(-distance / light.distance + 1.0f), light.decay);
	// 範囲減衰
	float cosAngle = dot(direction, light.direction);
	float falloffFactor = saturate((cosAngle - light.angle) / (light.falloffStart - light.angle));
	
	LightingData data;
	data.incoming.radiance = light.color.rgb * light.intensity * distanceDecayFactor * falloffFactor;
	data.incoming.direction = normalize(pixel.world - light.position);
	data.surface.albedo = pixel.color;
	data.surface.normal = pixel.normal;
	data.surface.shininess = pixel.shininess;
	data.surface.toCamera = normalize(camera - pixel.world);
	
	return data;
};

#endif // SPOT_LIGHTING_HLSLI
