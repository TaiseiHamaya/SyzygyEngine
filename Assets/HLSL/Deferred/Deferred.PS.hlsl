#include "Deferred.hlsli"
#include "Tools/PackNormalV2.hlsli"
#include "Tools/PackA2.hlsli"
#include "Tools/PackShininess.hlsli"

struct Material {
	float3 color;
	uint lightingType;
	float shininess;
	uint textureIndex;
	float3x3 uvTransform;
};

struct PixelShaderOutput {
	float4 albedoShading : SV_Target0; // 10bit*3+2bit unorm(xyz : Albedo, w : ShadingType)
	uint normalShininess : SV_Target1; // 11bit*2+10bit float(xy : ViewNormalPacked, z : Shininess)
};

StructuredBuffer<Material> gMaterial : register(t0, space0);
SamplerState gSampler : register(s0);

[earlydepthstencil]
PixelShaderOutput main(VertexShaderOutput input) {
	Material material = gMaterial[input.instance];
	
	PixelShaderOutput output = (PixelShaderOutput)0;
	// texture color
	const Texture2D<float4> texture = ResourceDescriptorHeap[material.textureIndex];
	float3 transformedUV = mul(float3(input.texcoord, 1.0f), material.uvTransform);
	
	float4 textureColor = texture.Sample(gSampler, transformedUV.xy);
	
	if (textureColor.a <= 0.05f) {
		discard;
	}
	
	// 出力
	output.albedoShading.xyz = textureColor.rgb * material.color.rgb;
	output.albedoShading.w = PackA2bit(material.lightingType); // packing
	output.normalShininess |= PackingNormalV2(input.normal);
	output.normalShininess |= PackShininess(material.shininess);
	return output;
}