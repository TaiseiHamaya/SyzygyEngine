#pragma once

#include <memory>

#include <Library/Math/Vector3.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

struct EmitterInstanceSettings;

/// <summary>
/// 初期方向の決定
/// </summary>
class ParticleEmissionDirection {
public:
	ParticleEmissionDirection() = default;
	virtual ~ParticleEmissionDirection() = default;

	SZG_CLASS_MOVE_ONLY(ParticleEmissionDirection)

public:
	/// <summary>
	/// 設定からの生成
	/// </summary>
	/// <param name="settings_">エミッタ設定</param>
	/// <returns></returns>
	static std::unique_ptr<ParticleEmissionDirection> Create(const EmitterInstanceSettings& settings_);

public:
	/// <summary>
	/// 初期方向の決定
	/// </summary>
	/// <param name="outward">形状の外向き方向</param>
	/// <returns></returns>
	virtual Vector3 decide(const Vector3& outward) const = 0;
};

/// <summary>
/// 固定方向
/// </summary>
class ConstantEmissionDirection final : public ParticleEmissionDirection {
public:
	explicit ConstantEmissionDirection(const Vector3& direction_);
	~ConstantEmissionDirection() override = default;

	SZG_CLASS_MOVE_ONLY(ConstantEmissionDirection)

public:
	/// <summary>
	/// 初期方向の決定
	/// </summary>
	/// <param name="outward">形状の外向き方向</param>
	/// <returns></returns>
	Vector3 decide(const Vector3& outward) const override;

private:
	Vector3 direction{ CVector3::BASIS_Y };
};

/// <summary>
/// 形状の外向き方向
/// </summary>
class ShapeEmissionDirection final : public ParticleEmissionDirection {
public:
	ShapeEmissionDirection() = default;
	~ShapeEmissionDirection() override = default;

	SZG_CLASS_MOVE_ONLY(ShapeEmissionDirection)

public:
	/// <summary>
	/// 初期方向の決定
	/// </summary>
	/// <param name="outward">形状の外向き方向</param>
	/// <returns></returns>
	Vector3 decide(const Vector3& outward) const override;
};

/// <summary>
/// 基準方向＋角度範囲
/// </summary>
class AngleRangeEmissionDirection final : public ParticleEmissionDirection {
public:
	AngleRangeEmissionDirection(const Vector3& baseDirection_, r32 angle_);
	~AngleRangeEmissionDirection() override = default;

	SZG_CLASS_MOVE_ONLY(AngleRangeEmissionDirection)

public:
	/// <summary>
	/// 初期方向の決定
	/// </summary>
	/// <param name="outward">形状の外向き方向</param>
	/// <returns></returns>
	Vector3 decide(const Vector3& outward) const override;

private:
	Vector3 baseDirection{ CVector3::BASIS_Y };
	r32 angle{ 0.5f };
};

}; // szg
