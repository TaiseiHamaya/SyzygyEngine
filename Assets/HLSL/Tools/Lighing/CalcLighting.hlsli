#ifndef CALC_LIGHTING_HLSLI
#define CALC_LIGHTING_HLSLI

struct LightingData {
	struct Surface {
		float3 toCamera; // 視線方向
		float3 albedo;   // ピクセルの基本色
		float3 normal;   // 法線ベクトル
		float shininess; // 鏡面反射係数
	} surface;

	struct Irradiance {
		float3 direction; // 入射方向
		float3 radiance;  // 到達光
	} incoming;
};

float3 BlinnPhongSpecular(LightingData data) {
	// specular
	float3 halfVector = normalize(-data.incoming.direction + data.surface.toCamera);
	float dotNormal = dot(data.surface.normal, halfVector);
	float specularPow = pow(saturate(dotNormal), data.surface.shininess);
	if (isnan(specularPow)) {
		specularPow = 0;
	}
	else {
		specularPow *= 2.0f;
	}
	return data.incoming.radiance * specularPow * data.surface.albedo;
};

float3 LambertDiffuse(LightingData data) {
	float NdotL = dot(data.surface.normal, -data.incoming.direction);
	float cos = saturate(NdotL);
	return data.incoming.radiance * cos * data.surface.albedo;
};

float3 HalfLambertDiffuse(LightingData data) {
	float NdotL = dot(data.surface.normal, -data.incoming.direction);
	float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
	return data.incoming.radiance * cos * data.surface.albedo;
};

#endif // CALC_LIGHTING_HLSLI
