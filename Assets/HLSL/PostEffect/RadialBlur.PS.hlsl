#include "FullscreenShader.hlsli"

struct BlurInfo {
	float2 center;
	float weight;
	float length;
	uint sampleCount;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<BlurInfo> gBlurInfo : register(b0);

float4 main(VertexShaderOutput input) : SV_TARGET0 {
	float2 blurDirection = (input.texcoord - gBlurInfo.center) * gBlurInfo.length / gBlurInfo.sampleCount;
	
	float weight[16];
	float totalWeight = 0;
	uint count;
	
	for (count = 0; count < gBlurInfo.sampleCount; ++count) {
		weight[count] = exp(-pow(float(count) / gBlurInfo.sampleCount, 2) / (gBlurInfo.weight * gBlurInfo.weight * 2));
		totalWeight += weight[count];
	}
	
	for (count = 0; count < gBlurInfo.sampleCount; ++count) {
		weight[count] = weight[count] / totalWeight;
	}
	
	float4 outputColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (count = 0; count < gBlurInfo.sampleCount; ++count) {
		outputColor += gTexture.Sample(gSampler, input.texcoord - blurDirection * count) * weight[count];
	}
	outputColor.a = 1.0f;
	return outputColor;
}