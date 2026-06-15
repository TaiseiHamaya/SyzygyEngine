#include "../Forward.hlsli"

struct TransformMatrix {
	float4x4 world;
};

struct CameraInformation {
	float4x4 viewProjection;
};

struct SkeletonMatrixPalette {
	float4x4 skeletonSpaceMatrix;
	float4x4 skeletonSpaceInv;
};

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	
	float4 weight : WEIGHT0;
	uint4 index : INDEX0;
};

ConstantBuffer<TransformMatrix> gTransformMatrix : register(b0);
ConstantBuffer<CameraInformation> gCameraMatrix : register(b1);
StructuredBuffer<SkeletonMatrixPalette> gSkeletonMatrixPalette : register(t1);

static const float4x4 wvp = mul(gTransformMatrix.world, gCameraMatrix.viewProjection);

struct SkinnedVertex {
	float4 position;
	float3 normal;
};

VertexShaderOutput main(VertexShaderInput input) {
	SkinnedVertex skinned;
	skinned.position = float4(0, 0, 0, 0);
	skinned.normal = float3(0, 0, 0);
	static const uint MaxJoint = 4;
	for (uint i = 0; i < MaxJoint; ++i) {
		uint index = input.index[i];
		float weight = input.weight[i];
		skinned.position += mul(input.position, gSkeletonMatrixPalette[index].skeletonSpaceMatrix) * weight;
		skinned.normal += mul(input.normal, (float3x3) transpose(gSkeletonMatrixPalette[index].skeletonSpaceInv)) * weight;
	}
	skinned.position.w = 1.0f;
	skinned.normal = normalize(skinned.normal);
	
	VertexShaderOutput output;

	output.world = mul(skinned.position, gTransformMatrix.world).xyz;
	output.position = mul(float4(output.world, 1.0f), gCameraMatrix.viewProjection);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(skinned.normal, (float3x3)gTransformMatrix.world));

	return output;
}
