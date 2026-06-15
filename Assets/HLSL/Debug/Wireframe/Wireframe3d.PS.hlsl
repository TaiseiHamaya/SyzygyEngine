struct Material {
	float4 color;
	int enableLighting;
	float4x4 uvTransform;
};

struct PixelShaderOutput {
	float4 color : SV_Target0;
};

struct DirectionalLight {
	float4 color; // 色
	float3 directon; // 向き
	float intensity; // 輝度
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main() {
	PixelShaderOutput output;
	output.color = float4(0, 0, 1.0f, 0);
	return output;
}