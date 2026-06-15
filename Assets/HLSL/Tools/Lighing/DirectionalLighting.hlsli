#ifndef DIRECTIONAL_LIGHTING_HLSLI
#define DIRECTIONAL_LIGHTING_HLSLI

#include "../Pixel.hlsli"
#include "CalcLighting.hlsli"

struct DirectionalLightBuffer {
	float3 color; // 色
	float intensity; // 輝度
	float3 directon; // 向き
};

LightingData CalcLightingData(Pixel pixel, float3 camera, DirectionalLightBuffer light) {
	LightingData data;
	data.incoming.radiance = light.color * light.intensity;
	data.incoming.direction = light.directon;
	data.surface.albedo = pixel.color;
	data.surface.normal = pixel.normal;
	data.surface.shininess = pixel.shininess;
	data.surface.toCamera = normalize(camera - pixel.world);
	
	return data;
};

#endif // DIRECTIONAL_LIGHTING_HLSLI
