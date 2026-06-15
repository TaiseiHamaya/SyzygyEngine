struct VertexShaderOutput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float fontSize : FONTSIZE0;
	uint materialIndex : MATERIAL0;
	uint glyphIndex : GLYPH0;
};

struct GlyphData {
	float3x3 uvMatrix;
	struct {
		float top;
		float left;
		float bottom;
		float right;
	} bounds;
};

struct StringBuffer {
	float4x4 world;
	float fontSize;
	float fontScale;
	float2 offset;
	uint glyphBindless;
};

GlyphData LoadGlyphData(uint glyphIndex, uint glyphBindless) {
	ByteAddressBuffer glyphsBindless = ResourceDescriptorHeap[glyphBindless];
	GlyphData glyph;
	glyph.uvMatrix[0] = asfloat(glyphsBindless.Load3(glyphIndex * 52 + 3 * 0 * 4));
	glyph.uvMatrix[1] = asfloat(glyphsBindless.Load3(glyphIndex * 52 + 3 * 1 * 4));
	glyph.uvMatrix[2] = asfloat(glyphsBindless.Load3(glyphIndex * 52 + 3 * 2 * 4));
	float4 temp = asfloat(glyphsBindless.Load4(glyphIndex * 52 + 3 * 3 * 4));
	glyph.bounds.top = temp.x;
	glyph.bounds.left = temp.y;
	glyph.bounds.bottom = temp.z;
	glyph.bounds.right = temp.w;
	return glyph;
}
