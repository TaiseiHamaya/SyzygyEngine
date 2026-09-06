#include "EmissionRandom.h"

#include <cmath>

#include <Library/Utility/Tools/RandomEngine.h>

using namespace szg;

r32 ParticleEmissionRandom::Range(r32 minValue, r32 maxValue) {
	if (minValue >= maxValue) {
		return minValue;
	}
	return std::lerp(minValue, maxValue, RandomEngine::Random01MOD());
}

Vector3 ParticleEmissionRandom::Range(const Vector3& minValue, const Vector3& maxValue) {
	return Vector3{ Range(minValue.x, maxValue.x), Range(minValue.y, maxValue.y), Range(minValue.z, maxValue.z) };
}

Vector2 ParticleEmissionRandom::Range(const Vector2& minValue, const Vector2& maxValue) {
	return Vector2{ Range(minValue.x, maxValue.x), Range(minValue.y, maxValue.y) };
}

ColorRGBA ParticleEmissionRandom::Range(const ColorRGBA& minValue, const ColorRGBA& maxValue) {
	return ColorRGBA{ Range(minValue.red, maxValue.red), Range(minValue.green, maxValue.green), Range(minValue.blue, maxValue.blue), Range(minValue.alpha, maxValue.alpha) };
}

Vector3 ParticleEmissionRandom::Direction() {
	r32 azimuth = RandomEngine::Random01MOD() * 2.0f * PI;
	r32 height = RandomEngine::Random01MOD() * 2.0f - 1.0f;
	r32 radius = sqrtf(1.0f - height * height);
	return Vector3{ radius * cosf(azimuth), height, radius * sinf(azimuth) };
}

Quaternion ParticleEmissionRandom::Rotation() {
	auto normal_pair = []() {
		r32 u1 = RandomEngine::Random01MOD();
		if (u1 <= 0.0f) {
			u1 = 1e-7f;
		}
		r32 u2 = RandomEngine::Random01MOD();
		r32 r = std::sqrt(-2.0f * std::log(u1));
		r32 theta = 2.0f * PI * u2;
		return std::pair<r32, r32>{ r* std::cos(theta), r* std::sin(theta) };
	};
	auto [x, y] = normal_pair();
	auto [z, w] = normal_pair();
	return Quaternion{ x, y, z, w }.normalize();
}
