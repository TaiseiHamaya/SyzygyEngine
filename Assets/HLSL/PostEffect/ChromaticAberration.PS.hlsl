#include "FullscreenShader.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
float2 gAberrationLevel : register(b0);

float4 main(VertexShaderOutput input) : SV_TARGET0 {
	float2 greenSamplePos = lerp(abs(gAberrationLevel), float2(1.0f, 1.0f) - abs(gAberrationLevel), input.texcoord);
	
	float4 red = gTexture.Sample(gSampler, greenSamplePos - gAberrationLevel);
	float4 green = gTexture.Sample(gSampler, greenSamplePos);
	float4 blue = gTexture.Sample(gSampler, greenSamplePos + gAberrationLevel);
	
	float4 outputColor = float4(red.r, green.g, blue.b, 1.0f);
	return outputColor;
}
