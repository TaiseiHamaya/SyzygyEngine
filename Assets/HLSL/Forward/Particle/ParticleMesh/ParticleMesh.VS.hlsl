#include "ParticleMesh.hlsli"

#include "Tools/MathTool.hlsli"

struct VertexShaderInput {
	float3 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

StructuredBuffer<ParticleTransformMatrix> gParticleMatrix : register(t0, space0);
ConstantBuffer<ParticleCameraInformation> gCameraMatrix : register(b0, space1);

VertexShaderOutput main(VertexShaderInput input, uint instance : SV_InstanceID) {
	float3 world = transform(input.position, gParticleMatrix[instance].world);
	const float3x3 worldIT = gParticleMatrix[instance].worldIT;

	VertexShaderOutput output;
	output.position = mul(float4(world, 1.0f), gCameraMatrix.viewProjection);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, worldIT));
	output.world = world;
	output.instance = instance;
	return output;
}
