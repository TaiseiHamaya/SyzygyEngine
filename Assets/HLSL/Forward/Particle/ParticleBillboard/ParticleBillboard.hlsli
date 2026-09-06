#ifndef PARTICLE_BILLBOARD_HLSLI
#define PARTICLE_BILLBOARD_HLSLI

#include "../../Forward.hlsli"

struct ParticleTransformMatrix {
	float4x3 world;
	float3x3 worldIT;
};

struct ParticleRectData {
	float2 size;
	float2 pivot;
	float angle;
	float pad;
};

struct ParticleCameraInformation {
	float4x4 viewProjection;
	float4x4 view;
};

#endif // PARTICLE_BILLBOARD_HLSLI
