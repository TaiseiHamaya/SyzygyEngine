#include "ParticleMesh.hlsli"

struct ParticleData {
	float4x4 world;
	float4x4 uvMatrix;
	float4 color;
};

struct CameraInformation {
	float4x4 viewProjection;
};

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};

StructuredBuffer<ParticleData> gParticleData : register(t0);
ConstantBuffer<CameraInformation> gCameraMatrix : register(b0);

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID) {
	VertexShaderOutput output;
	const float4x4 wvp = mul(gParticleData[instanceID].world, gCameraMatrix.viewProjection);
	output.position = mul(input.position, wvp);
	float3 transformedUV = mul(float3(input.texcoord, 1.0f), (float3x3) gParticleData[instanceID].uvMatrix);
	output.texcoord = transformedUV.xy / transformedUV.z;
	output.color = gParticleData[instanceID].color;
	return output;
}
