#include "PrimitiveGeometry.hlsli"

struct TransformMatrix {
	float4x4 world;
};

struct CameraInfomation {
	float4x4 viewProjection;
};

StructuredBuffer<TransformMatrix> gTransformMatrix : register(t0);
ConstantBuffer<CameraInfomation> gCameraMatrix : register(b0);

VertexShaderOutput main(float3 position : POSITION0, uint index : SV_InstanceID) {
	VertexShaderOutput output;
	const float4x4 wvp = mul(gTransformMatrix[index].world, gCameraMatrix.viewProjection);

	output.position = mul(float4(position, 1.0f), wvp);
	output.instance = index;
	return output;
}
