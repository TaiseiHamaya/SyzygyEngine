#include "Misc/PrimitiveGeometry/PrimitiveGeometry.hlsli"

#include "Tools/Lighing/PointLighting.hlsli"
#include "Tools/PackA2.hlsli"

struct Camera {
	float3 position;
	float4x4 viewInv;
	float4x4 projInv;
};

struct OutputTextureSize {
	float2 size;
	uint2 padding;
};

Texture2D<float4> gAlbedoShading : register(t0);
Texture2D<uint> gNormal : register(t1);
Texture2D<float> gDepth : register(t2);

StructuredBuffer<PointLightBuffer> gPointLight : register(t3);
ConstantBuffer<Camera> gCamera : register(b0);
ConstantBuffer<OutputTextureSize> gOutputSize : register(b1);

[earlydepthstencil]
float4 main(VertexShaderOutput input) : SV_TARGET {
	float4 output = float4(0, 0, 0, 0);
	
	const float2 texcoord = input.position.xy / gOutputSize.size;
	
	// sampling
	float4 albedoShading = gAlbedoShading.Load(input.position.xyz);
	uint normalViewShininess = gNormal.Load(input.position.xyz);
	float ndcDepth = gDepth.Load(input.position.xyz);
	
	// unpack
	uint shadingType = UnpackA2bit(albedoShading.a);
	Pixel pixel = UnpackPixel(
		albedoShading, normalViewShininess, ndcDepth,
		texcoord, gCamera.viewInv, gCamera.projInv
	);
	
	PointLightBuffer pointLight = gPointLight[input.instance];
	
	LightingData lightingData = CalcLightingData(pixel, gCamera.position, pointLight);	
	
	[branch]
	// ライティングなし
	if (shadingType == 0) {
		discard;
	}
	// Lambert
	else if (shadingType == 1) {
		output.rgb = BlinnPhongSpecular(lightingData) + LambertDiffuse(lightingData);
		output.a = 1.0f;
	}
	// Half Lambert
	else if (shadingType == 2) {
		output.rgb = BlinnPhongSpecular(lightingData) + HalfLambertDiffuse(lightingData);
		output.a = 1.0f;
	}
	// それ以外は異常値なので、黒を出力
	else {
		discard;
	}
	
	// 透明の場合は出力せず終了
	// サンプリングによっては0にならない場合があるためバッファを取る
	if (output.a <= 0.05f) {
		discard;
	}
	
	return output;
}