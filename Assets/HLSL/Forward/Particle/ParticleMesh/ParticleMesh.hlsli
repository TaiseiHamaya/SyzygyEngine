#ifndef PARTICLE_MESH_HLSLI
#define PARTICLE_MESH_HLSLI

#include "../../Forward.hlsli"

struct ParticleTransformMatrix {
	float4x3 world;
	float3x3 worldIT;
};

struct ParticleCameraInformation {
	float4x4 viewProjection;
	float4x4 view;
};

#endif // PARTICLE_MESH_HLSLI
