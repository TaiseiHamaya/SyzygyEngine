#include "FullscreenShader.hlsli"

VertexShaderOutput main( uint index : SV_vertexID, uint instance : SV_InstanceID ) {
	VertexShaderOutput output;
	const float2 vertex = float2(index % 2, index / 2);
	output.position = float4(float2(4.0f, -4.0f) * vertex + float2(-1.0f, 1.0f), 0.0f, 1.0f);
	output.texcoord = vertex.xy * float2(2.0f, 2.0f);
	output.instance = instance;
	return output;
}
