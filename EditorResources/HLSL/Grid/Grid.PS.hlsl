#include "Grid.hlsli"

float4 main(VSOut input) : SV_TARGET0 {
	float4 output = float4(input.color, 1.0f);
	
	return output;
}