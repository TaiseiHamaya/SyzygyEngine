#ifndef MATH_TOOL_HLSLI
#define MATH_TOOL_HLSLI

float3 transform(float3 position, float4x3 transform) {
	return mul(position, (float3x3)transform) + transform[3];
};

#endif // MATH_TOOL_HLSLI
