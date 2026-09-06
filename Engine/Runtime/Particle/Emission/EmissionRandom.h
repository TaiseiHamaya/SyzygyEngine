#pragma once

#include <Library/Math/ColorRGBA.h>
#include <Library/Math/Quaternion.h>
#include <Library/Math/Vector2.h>
#include <Library/Math/Vector3.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

/// <summary>
/// 放出時の乱数採取
/// </summary>
class ParticleEmissionRandom {
public:
	ParticleEmissionRandom() = delete;
	~ParticleEmissionRandom() = delete;

	SZG_CLASS_DELETED(ParticleEmissionRandom)

public:
	/// <summary>
	/// 範囲乱数
	/// </summary>
	/// <param name="minValue">最小</param>
	/// <param name="maxValue">最大</param>
	/// <returns></returns>
	static r32 Range(r32 minValue, r32 maxValue);

	/// <summary>
	/// 範囲乱数
	/// </summary>
	/// <param name="minValue">最小</param>
	/// <param name="maxValue">最大</param>
	/// <returns></returns>
	static Vector3 Range(const Vector3& minValue, const Vector3& maxValue);

	/// <summary>
	/// 範囲乱数
	/// </summary>
	/// <param name="minValue">最小</param>
	/// <param name="maxValue">最大</param>
	/// <returns></returns>
	static Vector2 Range(const Vector2& minValue, const Vector2& maxValue);

	/// <summary>
	/// 範囲乱数
	/// </summary>
	/// <param name="minValue">最小</param>
	/// <param name="maxValue">最大</param>
	/// <returns></returns>
	static ColorRGBA Range(const ColorRGBA& minValue, const ColorRGBA& maxValue);

	/// <summary>
	/// 単位球面上の方向
	/// </summary>
	/// <returns></returns>
	static Vector3 Direction();

	/// <summary>
	/// SO(3)上で一様な回転Quaternion
	/// </summary>
	/// <returns></returns>
	static Quaternion Rotation();
};

}; // szg
