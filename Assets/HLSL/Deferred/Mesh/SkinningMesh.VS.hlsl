#include "../Deferred.hlsli"

#include "Tools/MathTool.hlsli"

struct TransformMatrix {
	float4x3 world;
	float3x3 worldIT;
};

struct CameraInformation {
	float4x4 viewProjection;
	float4x4 view;
};

struct SkeletonMatrixPalette {
	float4x4 skeletonSpaceMatrix;
	float3x3 skeletonIT;
};

struct VertexShaderInput {
	float3 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	
	float4 weight : WEIGHT0;
	uint4 index : INDEX0;
};

StructuredBuffer<TransformMatrix> gTransformMatrix : register(t0, space0);
ConstantBuffer<CameraInformation> gCameraMatrix : register(b0, space1);
StructuredBuffer<SkeletonMatrixPalette> gSkeletonMatrixPalette : register(t1, space0);
uint gPaletteSize : register(b0);

struct SkinnedVertex {
	float3 position;
	float3 normal;
};

VertexShaderOutput main(VertexShaderInput input, uint instance : SV_InstanceID) {
	SkinnedVertex skinned;
	skinned.position = float3(0, 0, 0);
	skinned.normal = float3(0, 0, 0);
	static const uint MaxJoint = 4;
	for (uint i = 0; i < MaxJoint; ++i) {
		uint index = input.index[i] + gPaletteSize * instance;
		float weight = input.weight[i];
		skinned.position += mul(float4(input.position, 1.0f), gSkeletonMatrixPalette[index].skeletonSpaceMatrix).xyz * weight;
		skinned.normal += normalize(mul(input.normal, gSkeletonMatrixPalette[index].skeletonIT)) * weight;
	}
	skinned.normal = normalize(skinned.normal);

	float3 world = transform(skinned.position, gTransformMatrix[instance].world);
	const float3x3 worldIT = gTransformMatrix[instance].worldIT;
	
	VertexShaderOutput output;

	output.position = mul(float4(world, 1.0f), gCameraMatrix.viewProjection);
	output.texcoord = input.texcoord;
	float3 worldView = normalize(mul(skinned.normal, worldIT));
	output.normal = normalize(mul(worldView, (float3x3)gCameraMatrix.view));
	output.instance = instance;
	return output;
}
