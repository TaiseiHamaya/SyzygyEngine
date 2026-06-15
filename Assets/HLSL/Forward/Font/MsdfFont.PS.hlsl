#include "MsdfFont.hlsli"

struct Material {
	float4 color;
	uint lightingType;
	float shininess;
	uint textureIndex;
	float3x3 uvTransform;
};

struct PixelShaderOutput {
	float4 color : SV_Target0;
};

StructuredBuffer<Material> gMaterial : register(t0, space0);
StructuredBuffer<StringBuffer> gStringBuffer : register(t1, space2);
SamplerState gSampler : register(s0);

float median3(float3 v) {
	return max(min(v.x, v.y), min(max(v.x, v.y), v.z));
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	
	Material material = gMaterial[input.materialIndex];
	
	// texture color
	const Texture2D<float4> texture = ResourceDescriptorHeap[material.textureIndex];
	GlyphData glyph = LoadGlyphData(input.glyphIndex, gStringBuffer[input.materialIndex].glyphBindless);
	
	float3 uv = mul(float3(input.texcoord, 1.0f), glyph.uvMatrix);
	
	float4 mtsdf = texture.Sample(gSampler, uv.xy / uv.z);
	float dist = median3(mtsdf.rgb) - 0.5f;
	float fw = fwidth(dist);
	fw = max(fw, 1e-6);
	float alpha = saturate(dist / fw + 0.5);
	
	clip(alpha - 0.01f);
	
	// 出力
	output.color.rgb = material.color.rgb;
	output.color.a = alpha * material.color.a;
	return output;
}