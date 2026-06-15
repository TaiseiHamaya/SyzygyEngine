#include "../Forward.hlsli"

#include "Tools/MathTool.hlsli"

struct TransformMatrix {
	float4x3 world;
	float3x3 worldIT;
};

struct RectData {
	float2 size;
	float2 pivot;
	bool isFlipY;
};

struct CameraInformation {
	float4x4 viewProjection;
};

static const float2 Positions[] = {
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
};

static const float2 Texcoords[] = {
	float2(1.0f, 1.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 0.0f),
	float2(0.0f, 0.0f),
};

StructuredBuffer<TransformMatrix> gTransformMatrix : register(t0, space0);
StructuredBuffer<RectData> gRectData : register(t1, space0);
ConstantBuffer<CameraInformation> gCameraMatrix : register(b0, space1);

static const float3 Normal = float3(0.0f, 0.0f, 1.0f);

VertexShaderOutput main(uint index : SV_VertexID, uint instance : SV_InstanceID) {
	RectData data = gRectData[instance];
	float3 position = float3((Positions[index] - data.pivot) * data.size, 0.0f);
	float2 texcoord = Texcoords[index];
	
	if (data.isFlipY) {
		texcoord.y = 1.0f - texcoord.y;
	}

	float3 world = transform(position, gTransformMatrix[instance].world);
	const float3x3 worldIT = gTransformMatrix[instance].worldIT;
	
	VertexShaderOutput output;

	output.position = mul(float4(world, 1.0f), gCameraMatrix.viewProjection);
	output.texcoord = texcoord;
	output.normal = normalize(mul(Normal, worldIT));
	output.world = world;
	output.instance = instance;
	return output;
}
