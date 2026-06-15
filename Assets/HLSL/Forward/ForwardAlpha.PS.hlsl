#include "Forward.hlsli"

#include "Tools/Lighing/DirectionalLighting.hlsli"

struct Material {
	float4 color;
	uint lightingType;
	float shininess;
	uint textureIndex;
	float3x3 uvTransform;
};

struct Camera {
	float3 position;
};

StructuredBuffer<Material> gMaterial : register(t0, space0);
ConstantBuffer<Camera> gCamera : register(b0, space1);
StructuredBuffer<DirectionalLightBuffer> gDirectionalLight : register(t0, space2);
SamplerState gSampler : register(s0);

float4 main(VertexShaderOutput input) : SV_Target0 {
	float4 output = float4(0, 0, 0, 0);
	
	Material material = gMaterial[input.instance];
	const Texture2D<float4> texture = ResourceDescriptorHeap[material.textureIndex];
	
	float3 transformedUV = mul(float3(input.texcoord, 1.0f), material.uvTransform);
	float4 textureColor = texture.Sample(gSampler, transformedUV.xy);
	
	float alpha = textureColor.a * material.color.a;
	
	Pixel pixel;
	pixel.color = textureColor.rgb * material.color.rgb;
	pixel.normal = input.normal;
	pixel.shininess = material.shininess;
	pixel.world = input.world;
	
	uint numStructs;
	uint stride;
	
	gDirectionalLight.GetDimensions(numStructs, stride);
	// ライティングなし
	[branch]
	if (material.lightingType == 0) {
		output.rgb = pixel.color;
		output.a = alpha;
	}
	// Lambert
	else if (material.lightingType == 1) {
		for (uint i = 0; i < numStructs; i++) {
			DirectionalLightBuffer directionalLight = gDirectionalLight[i];
		
			LightingData lightingData = CalcLightingData(pixel, gCamera.position, directionalLight);
			output.rgb += BlinnPhongSpecular(lightingData) + LambertDiffuse(lightingData);
			output.a = alpha;
		}
	}
	// Half Lambert
	else if (material.lightingType == 2) {
		for (uint i = 0; i < numStructs; i++) {
			DirectionalLightBuffer directionalLight = gDirectionalLight[i];
		
			LightingData lightingData = CalcLightingData(pixel, gCamera.position, directionalLight);
			output.rgb += BlinnPhongSpecular(lightingData) + HalfLambertDiffuse(lightingData);
			output.a = alpha;
		}
	}
	
	// 透明の場合は出力せず終了
	// サンプリングによっては0にならない場合があるためバッファを取る
	clip(alpha - 0.005f);
	
	return output;
}