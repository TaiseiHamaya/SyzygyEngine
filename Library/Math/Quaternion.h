#pragma once

#include "Vector3.h"

#include "Basis.h"
#include "Matrix4x4.h"

class Quaternion final {
public:
	inline constexpr Quaternion() noexcept;
	~Quaternion() noexcept = default;

	/// <summary>
	/// クォータニオンのパラメータを直接構築して作成
	/// </summary>
	/// <param name="xyz_">虚部</param>
	/// <param name="w_">実部</param>
	inline constexpr Quaternion(const Vector3& xyz_, r32 w_) noexcept;

	/// <summary>
	/// クォータニオンのパラメータを直接構築して作成
	/// </summary>
	/// <param name="x">i成分</param>
	/// <param name="y">j成分</param>
	/// <param name="z">k成分</param>
	/// <param name="w">実部</param>
	inline constexpr Quaternion(r32 x, r32 y, r32 z, r32 w) noexcept;

	Quaternion(const Quaternion& rhs) noexcept = default;
	Quaternion(Quaternion&& rhs) noexcept = default;
	Quaternion& operator=(const Quaternion& rhs) noexcept = default;
	Quaternion& operator=(Quaternion&& rhs) noexcept = default;

private:
	Vector3 xyz; // 虚部ベクトル
	r32 w; // 実部

public:
	/// <summary>
	/// 回転軸と回転量を指定してQuaternionを作成
	/// </summary>
	/// <param name="axis">回転軸(Vector3)</param>
	/// <param name="angleAxis">回転量(Radian)</param>
	static Quaternion AngleAxis(const Vector3& axis, r32 angleAxis);

	/// <summary>
	/// オイラー角からクォータニオンを生成(ラジアン)
	/// </summary>
	/// <param name="pitch">X軸回転</param>
	/// <param name="yaw">Y軸回転</param>
	/// <param name="roll">Z軸回転</param>
	static Quaternion EulerRadian(r32 pitch, r32 yaw, r32 roll) noexcept;

	/// <summary>
	/// オイラー角からクォータニオンを生成(ラジアン)
	/// </summary>
	/// <param name="rotate">XYZ軸回転</param>
	static Quaternion EulerRadian(const Vector3& rotate) noexcept;

	/// <summary>
	/// オイラー角からクォータニオンを生成(度数法)
	/// </summary>
	/// <param name="rotate">XYZ軸回転</param>
	/// <returns></returns>
	static Quaternion EulerDegree(const Vector3& rotate) noexcept;

	/// <summary>
	/// オイラー角からクォータニオンを生成(度数法)
	/// </summary>
	/// <param name="pitch">X軸回転</param>
	/// <param name="yaw">Y軸回転</param>
	/// <param name="roll">Z軸回転</param>
	/// <returns></returns>
	static Quaternion EulerDegree(r32 pitch, r32 yaw, r32 roll) noexcept;


public:
	inline constexpr bool operator==(const Quaternion& rhs) const noexcept;
	inline constexpr bool operator!=(const Quaternion& rhs) const noexcept;
	inline constexpr Quaternion operator*(const Quaternion& rhs) const noexcept;
	inline constexpr Quaternion& operator*=(const Quaternion& rhs) noexcept;
	inline constexpr Quaternion operator*(r32 times) const noexcept;
	inline constexpr Quaternion& operator*=(r32 times) noexcept;
	inline constexpr friend Vector3 operator*(const Vector3& vector, const Quaternion& quaternion);

public: // メンバ関数
	/// <summary>
	/// 回転行列に変換
	/// </summary>
	/// <returns>回転行列Matrix4x4</returns>
	inline constexpr Matrix4x4 to_matrix() const noexcept;

	inline constexpr Basis to_basis() const noexcept;

	/// <summary>
	/// Quaternionベクトルの長さ[1]
	/// </summary>
	/// <returns>基本は1</returns>
	r32 length() const noexcept;

	/// <summary>
	/// 逆クォータニオンの取得
	/// </summary>
	/// <returns>逆Quaternion</returns>
	inline constexpr Quaternion inverse() const noexcept;

	/// <summary>
	/// クォータニオンの正規化
	/// </summary>
	/// <returns></returns>
	Quaternion normalize() const noexcept;

	/// <summary>
	/// ベクトル部の取得
	/// </summary>
	/// <returns></returns>
	inline constexpr const Vector3& vector() const noexcept;

	/// <summary>
	/// 実部の取得
	/// </summary>
	/// <returns></returns>
	inline constexpr const r32& real() const noexcept;

public: // グローバルメンバ関数
	/// <summary>
	/// fromからtoに回転するQuaternionを生成
	/// </summary>
	/// <param name="from">開始視点</param>
	/// <param name="to">終了視点</param>
	/// <returns></returns>
	static Quaternion FromToRotation(const Vector3& from, const Vector3& to);

	/// <summary>
	/// forward方向を向くQuaternionを生成
	/// </summary>
	/// <param name="forward">前方を表す正規化済みベクトル</param>
	/// <param name="upward">上方を表す正規化済みベクトル</param>
	/// <returns></returns>
	static Quaternion LookForward(const Vector3& forward, const Vector3& upward = CVector3::BASIS_Y);

	/// <summary>
	/// 球面線形補間
	/// </summary>
	/// <param name="internal">開始Quaternion</param>
	/// <param name="terminal">終了Quaternion</param>
	/// <param name="t">媒介変数</param>
	/// <returns>変換後Quaternion</returns>
	static Quaternion Slerp(const Quaternion& internal, const Quaternion& terminal, r32 t) noexcept;

	/// <summary>
	/// 球面線形補間を遠回りで行う
	/// </summary>
	/// <param name="internal">開始Quaternion</param>
	/// <param name="terminal">終了Quaternion</param>
	/// <param name="t">媒介変数</param>
	/// <returns></returns>
	static Quaternion SlerpFar(const Quaternion& internal, const Quaternion& terminal, r32 t) noexcept;

	/// <summary>
	/// 球面線形補間を回転軸基準で必ず時計回りを行う
	/// </summary>
	/// <param name="internal">開始Quaternion</param>
	/// <param name="terminal">終了Quaternion</param>
	/// <param name="t">媒介変数</param>
	/// <param name="axis">回転軸</param>
	/// <returns></returns>
	static Quaternion SlerpClockwise(const Quaternion& internal, const Quaternion& terminal, r32 t, const Vector3& axis = CVector3::UP) noexcept;
};

inline constexpr Quaternion::Quaternion() noexcept : Quaternion{ 0,0,0,1 } {}

inline constexpr Quaternion::Quaternion(const Vector3& xyz_, r32 w_) noexcept :
	xyz(xyz_),
	w(w_) {
}

inline constexpr Quaternion::Quaternion(r32 x, r32 y, r32 z, r32 w) noexcept :
	xyz(Vector3{ x,y,z }),
	w(w) {
}

inline constexpr bool Quaternion::operator==(const Quaternion& rhs) const noexcept {
	return xyz == rhs.xyz && w == rhs.w;
}

inline constexpr bool Quaternion::operator!=(const Quaternion& rhs) const noexcept {
	return !(*this == rhs);
}

inline constexpr Quaternion Quaternion::operator*(const Quaternion& rhs) const noexcept {
	Vector3 resultV = rhs.xyz * w + xyz * rhs.w + Vector3::Cross(xyz, rhs.xyz);
	return Quaternion{
		resultV, w * rhs.w - Vector3::Dot(xyz, rhs.xyz)
	};
}

inline constexpr Quaternion& Quaternion::operator*=(const Quaternion& rhs) noexcept {
	*this = rhs * *this;
	return *this;
}

inline constexpr Quaternion Quaternion::operator*(r32 times) const noexcept {
	return { xyz * times, w * times };
}

inline constexpr Quaternion& Quaternion::operator*=(r32 times) noexcept {
	*this = *this * times;
	return *this;
}

inline constexpr Matrix4x4 Quaternion::to_matrix() const noexcept {
	r32 xx = xyz.x * xyz.x;
	r32 xy = xyz.x * xyz.y;
	r32 xz = xyz.x * xyz.z;
	r32 xw = xyz.x * w;
	r32 yy = xyz.y * xyz.y;
	r32 yz = xyz.y * xyz.z;
	r32 yw = xyz.y * w;
	r32 zz = xyz.z * xyz.z;
	r32 zw = xyz.z * w;
	r32 ww = w * w;

	return {
		{{ww + xx - yy - zz, 2 * (xy + zw), 2 * (xz - yw), 0},
		{2 * (xy - zw),ww - xx + yy - zz, 2 * (yz + xw), 0},
		{2 * (xz + yw), 2 * (yz - xw), ww - xx - yy + zz , 0},
		{0,0,0,1}}
	};
}

inline constexpr Basis Quaternion::to_basis() const noexcept {
	r32 xx = xyz.x * xyz.x;
	r32 xy = xyz.x * xyz.y;
	r32 xz = xyz.x * xyz.z;
	r32 xw = xyz.x * w;
	r32 yy = xyz.y * xyz.y;
	r32 yz = xyz.y * xyz.z;
	r32 yw = xyz.y * w;
	r32 zz = xyz.z * xyz.z;
	r32 zw = xyz.z * w;
	r32 ww = w * w;

	return {
		{ww + xx - yy - zz, 2 * (xy + zw), 2 * (xz - yw)},
		{2 * (xy - zw),ww - xx + yy - zz, 2 * (yz + xw)},
		{2 * (xz + yw), 2 * (yz - xw), ww - xx - yy + zz}
	};
}

inline constexpr Quaternion Quaternion::inverse() const noexcept {
	return { -xyz.x, -xyz.y, -xyz.z, w };
}

inline constexpr const Vector3& Quaternion::vector() const noexcept {
	return xyz;
}

inline constexpr const r32& Quaternion::real() const noexcept {
	return w;
}

namespace CQuaternion {

constexpr Quaternion IDENTITY{ 0,0,0,1 }; // 回転なし
constexpr Quaternion BACK_X{ 1,0,0,0 }; // X軸に180度回転する
constexpr Quaternion BACK_Y{ 0,1,0,0 }; // Y軸に180度回転する
constexpr Quaternion BACK_Z{ 0,0,1,0 }; // Z軸に180度回転する

};

/// <summary>
/// ベクトルにQuaternion回転を適用する
/// </summary>
/// <param name="vector">元のベクトル</param>
/// <param name="quaternion">回転Quaternion</param>
/// <returns></returns>
inline constexpr Vector3 operator*(const Vector3& vector, const Quaternion& quaternion) {
	Quaternion vectorQuaternion = Quaternion{ vector, 0.0f };
	return (quaternion * vectorQuaternion * quaternion.inverse()).xyz;
}

