#include "MsdfFont.hlsli"

static const float2 Positions[] = {
	float2(0.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(-1.0f, 0.0f),
	float2(-1.0f, 1.0f),
};

static const float2 Texcoords[] = {
	float2(0.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 0.0f),
	float2(1.0f, 1.0f),
};

struct CharBufferVS {
	float2 rectOffset;
	float2 size;
	uint glyphIndex;
	uint instanceIndex;
};

struct CameraInfomation {
	float4x4 viewProjection;
	float4x4 view;
};

ConstantBuffer<CameraInfomation> gCameraMatrix : register(b0);
StructuredBuffer<CharBufferVS> gCharBufferVS : register(t0, space2);
StructuredBuffer<StringBuffer> gStringBuffer : register(t1, space2);

VertexShaderOutput main(uint vertexId : SV_VertexID, uint instanceId : SV_InstanceID) {
	VertexShaderOutput output;
	
	float2 temp = Positions[vertexId];
	CharBufferVS charBuffer = gCharBufferVS[instanceId];
	StringBuffer stringBuffer = gStringBuffer[charBuffer.instanceIndex];
	GlyphData glyph = LoadGlyphData(charBuffer.glyphIndex, stringBuffer.glyphBindless);
	
	float2 vertex = ((charBuffer.rectOffset + temp * charBuffer.size) + stringBuffer.offset) * stringBuffer.fontScale;
	float4 world = mul(float4(vertex, 0.0, 1.0f), stringBuffer.world);

	float4x4 vp = gCameraMatrix.viewProjection;

	output.position = mul(world, vp);
	output.texcoord = Texcoords[vertexId];
	output.fontSize = stringBuffer.fontSize;
	output.materialIndex = charBuffer.instanceIndex;
	output.glyphIndex = charBuffer.glyphIndex;
	
	return output;
}
