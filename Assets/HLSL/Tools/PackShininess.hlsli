#ifndef PACK_SHININESS_HLSLI
#define PACK_SHININESS_HLSLI

#define SHININESS_BITS 8
#define SHININESS_BIT_MASK ((1u << SHININESS_BITS) - 1)

uint PackShininess(float shininess) {
	return saturate(shininess / 400) * SHININESS_BIT_MASK;

}

uint UnpackShininess(uint packed) {
	return (packed & SHININESS_BIT_MASK) / (float)SHININESS_BIT_MASK * 400;
}

#endif // !PACK_SHININESS_HLSLI
