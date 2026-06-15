#include "Sprite.hlsli"

struct VertexShaderInput {
	float2 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};

float4x4 gTransformMatrix : register(b0);

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	output.position = mul(float4(input.position, 1.0f, 1.0f), gTransformMatrix);
	output.texcoord = input.texcoord;
	return output;
}