#ifndef TO_LINEAR_Z_BUFFER_HLSLI
#define TO_LINEAR_Z_BUFFER_HLSLI

float ToLinearZBuffer(float ndcZ, float near, float far) {
	return far * near / (far - ndcZ * (far - near));
}

#endif // !TO_LINEAR_Z_BUFFER_HLSLI
