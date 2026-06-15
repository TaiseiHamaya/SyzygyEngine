#include "FullscreenShader.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
uint gIsGray : register(b0);

float4 main(VertexShaderOutput input) : SV_TARGET0 {
	float4 outputColor;
	float4 sampleColor = gTexture.Sample(gSampler, input.texcoord);
	if (gIsGray) {
		float y = 0.3f * sampleColor.r + 0.58f* sampleColor.g + 0.12f * sampleColor.b;
		outputColor = float4(y, y, y, 1.0f);
	}
	else {
		outputColor = sampleColor;
	}
	return outputColor;
}