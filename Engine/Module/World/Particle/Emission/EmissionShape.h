#pragma once

#include <memory>

#include <Library/Math/Vector3.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

struct EmitterInstanceSettings;

/// <summary>
/// 放出形状に応じた初期位置の決定
/// </summary>
class ParticleEmissionShape {
public:
	ParticleEmissionShape() = default;
	virtual ~ParticleEmissionShape() = default;

	SZG_CLASS_MOVE_ONLY(ParticleEmissionShape)

public:
	/// <summary>
	/// 設定からの生成
	/// </summary>
	/// <param name="settings_">エミッタ設定</param>
	/// <returns></returns>
	static std::unique_ptr<ParticleEmissionShape> Create(const EmitterInstanceSettings& settings_);

public:
	/// <summary>
	/// 初期位置の抽出
	/// </summary>
	/// <param name="localPosition">形状内位置</param>
	/// <param name="outward">外向き方向</param>
	virtual void sample(Vector3& localPosition, Vector3& outward) const = 0;
};

/// <summary>
/// 点放出
/// </summary>
class PointEmissionShape final : public ParticleEmissionShape {
public:
	PointEmissionShape() = default;
	~PointEmissionShape() override = default;

	SZG_CLASS_MOVE_ONLY(PointEmissionShape)

public:
	/// <summary>
	/// 初期位置の抽出
	/// </summary>
	/// <param name="localPosition">形状内位置</param>
	/// <param name="outward">外向き方向</param>
	void sample(Vector3& localPosition, Vector3& outward) const override;
};

/// <summary>
/// 球放出
/// </summary>
class SphereEmissionShape final : public ParticleEmissionShape {
public:
	explicit SphereEmissionShape(r32 radius_);
	~SphereEmissionShape() override = default;

	SZG_CLASS_MOVE_ONLY(SphereEmissionShape)

public:
	/// <summary>
	/// 初期位置の抽出
	/// </summary>
	/// <param name="localPosition">形状内位置</param>
	/// <param name="outward">外向き方向</param>
	void sample(Vector3& localPosition, Vector3& outward) const override;

private:
	r32 radius{ 1.0f };
};

/// <summary>
/// 円錐放出
/// </summary>
class ConeEmissionShape final : public ParticleEmissionShape {
public:
	ConeEmissionShape(r32 radius_, const Vector3& direction_, r32 angle_);
	~ConeEmissionShape() override = default;

	SZG_CLASS_MOVE_ONLY(ConeEmissionShape)

public:
	/// <summary>
	/// 初期位置の抽出
	/// </summary>
	/// <param name="localPosition">形状内位置</param>
	/// <param name="outward">外向き方向</param>
	void sample(Vector3& localPosition, Vector3& outward) const override;

private:
	r32 radius{ 1.0f };
	Vector3 direction{ CVector3::BASIS_Y };
	r32 angle{ 0.5f };
};

/// <summary>
/// 箱放出
/// </summary>
class BoxEmissionShape final : public ParticleEmissionShape {
public:
	BoxEmissionShape(const Vector3& size_, const Vector3& offset_);
	~BoxEmissionShape() override = default;

	SZG_CLASS_MOVE_ONLY(BoxEmissionShape)

public:
	/// <summary>
	/// 初期位置の抽出
	/// </summary>
	/// <param name="localPosition">形状内位置</param>
	/// <param name="outward">外向き方向</param>
	void sample(Vector3& localPosition, Vector3& outward) const override;

private:
	Vector3 size{ CVector3::ONE };
	Vector3 offset{ CVector3::ZERO };
};

}; // szg
