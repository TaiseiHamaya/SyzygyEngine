#include "FullscreenShader.hlsli"

Texture2D<float4> gTexture : register(t0);
Texture2D<float> gDepth : register(t1);
SamplerState gLinearSampler : register(s0);
SamplerState gPointSampler : register(s1);

static const float2 Index3x3[3][3] = {
	{ { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
	{ { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
	{ { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } },
};

static const float KernerBase = 1.0f / 6.0f;

static const float3x3 HorizontalKerner = {
	{ -KernerBase, 0.0f, KernerBase },
	{ -KernerBase, 0.0f, KernerBase },
	{ -KernerBase, 0.0f, KernerBase }
};
static const float3x3 VerticalKerner = {
	{ -KernerBase, -KernerBase, -KernerBase },
	{ 0.0f, 0.0f, 0.0f },
	{ KernerBase, KernerBase, KernerBase }
};
static const float2 UVStepSize = float2(1.0f / 1280.0f, 1.0f / 720.0f);

float ZBufferToLinear(float ndcZ, float near, float far) {
	return ndcZ * near / (far - ndcZ * (far - near));
}

float4 main(VertexShaderOutput input) : SV_TARGET0 {
	float2 difference = float2(0.0f, 0.0f);
		
	for (int x = 0; x < 3; ++x) {
		for (int y = 0; y < 3; ++y) {
			float2 texcoord = input.texcoord + Index3x3[x][y] * UVStepSize;
			float viewZ = ZBufferToLinear(gDepth.Sample(gPointSampler, texcoord), 0.1f, 1000.0f);
			difference.x += viewZ * HorizontalKerner[x][y];
			difference.y += viewZ * VerticalKerner[x][y];
		}
	}
	
	float weight = saturate(length(difference));
	
	if (weight > 0.0005f) {
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else {
		return gTexture.Sample(gLinearSampler, input.texcoord);
	}
}