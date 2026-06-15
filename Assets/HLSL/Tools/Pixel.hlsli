#ifndef DEFERRED_PIXEL_HLSLI
#define DEFERRED_PIXEL_HLSLI

#include "PackNormalV2.hlsli"
#include "PackShininess.hlsli"
#include "ToLinearZBuffer.hlsli"

struct Pixel {
	float3 color;
	float3 normal;
	float shininess;
	float3 world;
};

Pixel UnpackPixel(float4 tex1, uint tex2, float ndcDepth, float2 texcoord, float4x4 cameraViewInv, float4x4 cameraProjInv) {
	Pixel pixel;
	// unpack
	pixel.color = tex1.rgb;
	pixel.normal = mul(UnpackingNormaV2(tex2), (float3x3)cameraViewInv);
	pixel.shininess = UnpackShininess(tex2);
	float3 ndc = float3(texcoord * 2 - 1, ndcDepth);
	ndc.y *= -1;
	float4 view = mul(float4(ndc, 1.0f), cameraProjInv);
	float4 worldH = mul(view, cameraViewInv);
	pixel.world = worldH.xyz / worldH.w;
	
	return pixel;
}

#endif // DEFERRED_PIXEL_HLSLI
