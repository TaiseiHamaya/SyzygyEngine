#ifndef TOOLS_PACKNORMALV2_HLSLI
#define TOOLS_PACKNORMALV2_HLSLI

#define NORMAL_BITS 12
#define NORMAL_BIT_MASK ((1u << NORMAL_BITS) - 1)

#define OTHER_BIT (32 - NORMAL_BITS * 2)

uint PackSnorm12Bit(float value) {
	return (uint)(saturate(value * 0.5f + 0.5f) * NORMAL_BIT_MASK);
}

uint PackingNormalV2(float3 viewNormal) {
	return
	PackSnorm12Bit(viewNormal.x) << (OTHER_BIT + NORMAL_BITS) |
	PackSnorm12Bit(viewNormal.y) << OTHER_BIT;

}

float3 UnpackingNormaV2(uint viewNormalPacked) {
	float2 viewNormal = float2(
		((viewNormalPacked >> (OTHER_BIT + NORMAL_BITS * 1)) & NORMAL_BIT_MASK) / (float)NORMAL_BIT_MASK * 2.0f - 1.0f,
		((viewNormalPacked >> (OTHER_BIT + NORMAL_BITS * 0)) & NORMAL_BIT_MASK) / (float)NORMAL_BIT_MASK * 2.0f - 1.0f
	);
	float z = -sqrt(1.0f - saturate(dot(viewNormal, viewNormal)));
	return normalize(float3(viewNormal, z));
}

#endif // TOOLS_PACKNORMALV2_HLSLI
