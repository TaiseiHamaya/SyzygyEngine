#include "FullscreenShader.hlsli"

#include "Tools/PackNormalV2.hlsli"
#include "Tools/PackA2.hlsli"
#include "Tools/PackShininess.hlsli"
#include "Tools/ToLinearZBuffer.hlsli"

Texture2D<float4> gAlbedoShading : register(t0);

[earlydepthstencil]
float4 main(VertexShaderOutput input) : SV_TARGET {
	float4 albedoShading = gAlbedoShading.Load(input.position.xyz);
	float3 albedo = albedoShading.rgb;
	uint shadingType = UnpackA2bit(albedoShading.a);
	
	float4 output = float4(0, 0, 0, 0);
	// ライティングなし
	if (shadingType == 0) {
		output = float4(albedo, 1);
	}
	else {
		discard;
	}
	return output;
}