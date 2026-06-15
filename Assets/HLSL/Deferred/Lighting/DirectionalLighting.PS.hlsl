#include "FullscreenShader.hlsli"

#include "Tools/Lighing/DirectionalLighting.hlsli"
#include "Tools/PackA2.hlsli"

struct Camera {
	float3 position;
	float4x4 viewInv;
	float4x4 projInv;
};

Texture2D<float4> gAlbedoShading : register(t0);
Texture2D<uint> gNormal : register(t1);
Texture2D<float> gDepth : register(t2);

StructuredBuffer<DirectionalLightBuffer> gDirectionalLight : register(t3);
ConstantBuffer<Camera> gCamera : register(b0);

[earlydepthstencil]
float4 main(VertexShaderOutput input) : SV_TARGET {
	float4 output = float4(0, 0, 0, 0);
	
	// sampling
	float4 albedoShading = gAlbedoShading.Load(input.position.xyz);
	uint normalViewShininess = gNormal.Load(input.position.xyz);
	float ndcDepth = gDepth.Load(input.position.xyz);
	
	uint shadingType = UnpackA2bit(albedoShading.a);
	Pixel pixel = UnpackPixel(
		albedoShading, normalViewShininess, ndcDepth,
		input.texcoord.xy, gCamera.viewInv, gCamera.projInv
	);
	
	DirectionalLightBuffer directionalLight = gDirectionalLight[input.instance];

	LightingData lightingData = CalcLightingData(pixel, gCamera.position, directionalLight);
	
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
	// それ以外は異常値なので、discard
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