#include "../Deferred.hlsli"

#include <Tools/MathTool.hlsli>

struct TransformMatrix {
	float4x3 world;
	float3x3 worldIT;
};

struct CameraInformation {
	float4x4 viewProjection;
	float4x4 view;
};

struct VertexShaderInput {
	float3 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

StructuredBuffer<TransformMatrix> gTransformMatrix : register(t0, space0);
ConstantBuffer<CameraInformation> gCameraMatrix : register(b0, space1);

VertexShaderOutput main(VertexShaderInput input, uint instance : SV_InstanceID) {
	float3 world = transform(input.position, gTransformMatrix[instance].world);
	const float3x3 worldIT = gTransformMatrix[instance].worldIT;
	
	VertexShaderOutput output;

	output.position = mul(float4(world, 1.0f), gCameraMatrix.viewProjection);
	output.texcoord = input.texcoord;
	float3 worldView = normalize(mul(input.normal, worldIT));
	output.normal = normalize(mul(worldView, (float3x3)gCameraMatrix.view));
	output.instance = instance;
	return output;
}
