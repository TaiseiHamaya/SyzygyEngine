#include "Grid.hlsli"

struct Data {
	float2 center;
	float scale;
};

struct CameraProjection {
	float4x4 viewProjection;
};

ConstantBuffer<Data> gData : register(b0);
ConstantBuffer<CameraProjection> gCamera : register(b1);

static const uint GRID_SIZE = 20;
static const float HALF_GRID_SIZE = GRID_SIZE / 2;
static const uint NUM_LINES = GRID_SIZE + 1;
static const uint INSTANCES = NUM_LINES * 2;
static const uint TOTAL_VERTICES = INSTANCES * 2;

VSOut main(uint vertex : SV_VertexID, uint instance : SV_InstanceID) {
	VSOut output;
	
	uint rowcol = instance / NUM_LINES;
	
	float lp = float(instance % NUM_LINES) / GRID_SIZE;
	
	float row = lerp(vertex, lp, rowcol);
	float col = lerp(lp, vertex, rowcol);

	float2 base = float2(
		lerp(-HALF_GRID_SIZE, HALF_GRID_SIZE, row),
		lerp(-HALF_GRID_SIZE, HALF_GRID_SIZE, col)
	);
	
	float2 world = base * gData.scale + gData.center;
	
	output.position = mul(float4(world.x, 0.0f, world.y, 1.0f), gCamera.viewProjection);
	
	// 色決定に条件式を使わない黒魔術
	float threshold = 1e-2f;
	float isRowLine = step(instance, NUM_LINES - 0.5f);
	float worldCoord = lerp(world.x, world.y, isRowLine);
	float isAxisLine = step(abs(worldCoord), threshold);
	float isMajorGridLine = step(abs(worldCoord % (gData.scale * 10)), threshold);
	
	float3 axisColor = lerp(float3(0.0f, 0.0f, 1.0f), float3(1.0f, 0.0f, 0.0f), isRowLine);
	float3 gridColor = lerp(float3(0.1f, 0.1f, 0.1f), float3(0.6f, 0.6f, 0.6f), isMajorGridLine);
	output.color = lerp(gridColor, axisColor, isAxisLine);

	return output;
}
