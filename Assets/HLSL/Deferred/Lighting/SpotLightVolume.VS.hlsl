struct TransformMatrix {
	float4x4 world;
};

struct CameraInformation {
	float4x4 viewProjection;
};

struct VertexShaderInput {
	float3 position : POSITION0;
};

ConstantBuffer<TransformMatrix> gTransformMatrix : register(b0);
ConstantBuffer<CameraInformation> gCameraMatrix : register(b1);

static const float4x4 wvp = mul(gTransformMatrix.world, gCameraMatrix.viewProjection);

float4 main(VertexShaderInput input) : SV_Position {
	return mul(float4(input.position, 1.0f), wvp);
}
