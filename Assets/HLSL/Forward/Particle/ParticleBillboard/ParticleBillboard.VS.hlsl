#include "ParticleBillboard.hlsli"

#include "Tools/MathTool.hlsli"

static const float2 Positions[] = {
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
};

static const float2 Texcoords[] = {
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
};

StructuredBuffer<ParticleTransformMatrix> gParticleMatrix : register(t0, space0);
StructuredBuffer<ParticleRectData> gParticleRectData : register(t1, space0);
ConstantBuffer<ParticleCameraInformation> gCameraMatrix : register(b0, space1);

static const float3 Normal = float3(0.0f, 0.0f, 1.0f);

VertexShaderOutput main(uint index : SV_VertexID, uint instance : SV_InstanceID) {
	ParticleRectData data = gParticleRectData[instance];
	float rotateCos = cos(data.angle);
	float rotateSin = sin(data.angle);
	float2 planar = (Positions[index] - data.pivot) * data.size;
	planar = float2(planar.x * rotateCos - planar.y * rotateSin, planar.x * rotateSin + planar.y * rotateCos);
	float3 position = float3(planar, 0.0f);

	float3 world = transform(position, gParticleMatrix[instance].world);
	const float3x3 worldIT = gParticleMatrix[instance].worldIT;

	VertexShaderOutput output;
	output.position = mul(float4(world, 1.0f), gCameraMatrix.viewProjection);
	output.texcoord = Texcoords[index];
	output.normal = normalize(mul(Normal, worldIT));
	output.world = world;
	output.instance = instance;
	return output;
}
