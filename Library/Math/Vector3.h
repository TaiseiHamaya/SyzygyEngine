#pragma once

#include <array>
#include <limits>

/// <summary>
/// 3次元ベクトルクラス
/// <param name="x">x方向のベクトル</param>
/// <param name="y">y方向のベクトル</param>
/// <param name="z">z方向のベクトル</param>
/// </summary>
class Vector3 final { // 3次元ベクトル
public:
	constexpr Vector3() noexcept;
	constexpr Vector3(r32 x, r32 y, r32 z) noexcept;
	constexpr Vector3(const Vector3& vec) noexcept = default;
	constexpr Vector3(Vector3&& vec) noexcept = default;

	~Vector3() noexcept = default;

public:
	// ------------------基礎構造------------------
	r32 x; // X座標
	r32 y; // Y座標
	r32 z; // Z座標

public:
	// ------------------operator------------------
	inline constexpr Vector3& operator=(const Vector3& vec) noexcept = default;
	inline constexpr Vector3& operator=(Vector3&& vec) noexcept = default;
	inline constexpr Vector3 operator+(void) const noexcept;
	inline constexpr Vector3 operator-(void) const noexcept;
	inline constexpr Vector3 operator+(const Vector3& opr) const noexcept;
	inline constexpr Vector3 operator-(const Vector3& opr) const noexcept;
	inline constexpr Vector3 operator*(r32 times) const noexcept;
	inline constexpr Vector3 operator/(r32 times) const noexcept;
	inline constexpr Vector3& operator+=(const Vector3& opr) noexcept;
	inline constexpr Vector3& operator-=(const Vector3& opr) noexcept;
	inline constexpr Vector3& operator*=(r32 times) noexcept;
	inline constexpr Vector3& operator/=(r32 times) noexcept;
	inline constexpr bool operator==(const Vector3& opr) const noexcept;
	inline constexpr bool operator!=(const Vector3& opr) const noexcept;
	inline constexpr const r32& operator[](size_t i) const;
	inline constexpr r32& operator[](size_t i);


public:
	// ------------------便利関数------------------

	/// <summary>
	/// ベクトルの長さを出す
	/// </summary>
	/// <returns></returns>
	r32 length(void) const noexcept;

	/// <summary>
	/// ベクトルの正規化
	/// </summary>
	/// <returns></returns>
	Vector3 normalize(void) const noexcept(false);

	/// <summary>
	/// ベクトルの正規化
	/// </summary>
	/// <param name="disapproval">範囲外だった場合の戻り値</param>
	/// <param name="tolerance">lengthの許容値</param>
	/// <returns></returns>
	Vector3 normalize_safe(const Vector3& disapproval = { 1,0,0 }, r32 tolerance = 1e-4f) const noexcept;

public:
	// ------------------static関数------------------

	/// <summary>
	/// 2つのベクトルの内積を計算
	/// <param name="vector1">3次元ベクトル1</param>
	/// <param name="vector2">3次元ベクトル2</param>
	/// </summary>
	static constexpr r32 Dot(const Vector3& vector1, const Vector3& vector2) noexcept;

	/// <summary>
	/// 2つのベクトルの外積を計算
	/// <param name="vector1">3次元ベクトル1</param>
	/// <param name="vector2">3次元ベクトル2</param>
	/// </summary>
	static constexpr Vector3 Cross(const Vector3& vector1, const Vector3& vector2) noexcept;

	/// <summary>
	/// ベクトルの大きさを計算
	/// <param name="vector">終点の座標</param>
	/// </summary>
	static r32 Length(const Vector3& vector) noexcept;

	/// <summary>
	/// ベクトルの大きさを計算
	/// <param name="vector1">終点の座標</param>
	/// <param name="vector2">始点の座標</param>
	/// </summary>
	static r32 Length(const Vector3& vector1, const Vector3& vector2) noexcept;

	/// <summary>
	/// ベクトルの正規化
	/// <param name="vector1">終点の座標</param>
	/// </summary>
	static Vector3 Normalize(const Vector3& vector);

	/// <summary>
	/// ベクトルの正規化
	/// <param name="vector1">始点の座標</param>
	/// <param name="vector2">終点の座標</param>
	/// </summary>
	static Vector3 Normalize(const Vector3& vectorFrom, const Vector3& vectorTo);

	/// <summary>
	/// 2つのベクトルを加算
	/// <param name="vector1">足されるベクトル</param>
	/// <param name="vector2">足すベクトル</param>
	/// </summary>
	static constexpr Vector3 Add(const Vector3& vector1, const Vector3& vector2) noexcept;

	/// <summary>
	/// 2つのベクトルを減算
	/// <param name="vector1">引かれるベクトル</param>
	/// <param name="vector2">引くベクトル</param>
	/// </summary>
	static constexpr Vector3 Subtract(const Vector3& vector1, const Vector3& vector2) noexcept;

	/// <summary>
	/// ベクトルを定数倍
	/// <param name="vector">乗算されるベクトル</param>
	/// <param name="times">係数</param>
	/// </summary>
	static constexpr Vector3 Multiply(const Vector3& vector, const r32& times) noexcept;

	/// <summary>
	/// ベクトルを定数倍
	/// <param name="vector1">乗算されるベクトル</param>
	/// <param name="times">係数</param>
	/// </summary>
	static constexpr Vector3 Multiply(const Vector3& vector, const Vector3& times) noexcept;

	/// <summary>
	/// 2つの位置ベクトルの間の座標
	/// <param name="from">始点の位置ベクトル</param>
	/// <param name="to">終点の位置ベクトル</param>
	/// <param name="t">割合T</param>
	/// </summary>
	static constexpr Vector3 Lerp(const Vector3& from, const Vector3& to, r32 t) noexcept;

	static constexpr Vector3 LerpElement(const Vector3& from, const Vector3& to, const Vector3& t) noexcept;

	/// <summary>
	/// 2次ベジエ曲線
	/// </summary>
	/// <param name="initial">始点</param>
	/// <param name="control">制御点</param>
	/// <param name="terminal">終点</param>
	/// <param name="t">媒介変数</param>
	/// <returns></returns>
	static constexpr Vector3 Bezier(const Vector3& initial, const Vector3& control, const Vector3& terminal, const r32 t);

	/// <summary>
	/// ベクトルの各要素の絶対値化
	/// </summary>
	/// <param name="vector">変化させるベクトル</param>
	/// <returns></returns>
	static Vector3 Abs(const Vector3& vector) noexcept;

	/// <summary>
	/// ベクトル射影
	/// </summary>
	/// <param name="vector">射影するベクトル</param>
	/// <param name="onto">射影対象の単位ベクトル</param>
	/// <returns></returns>
	static Vector3 Projection(const Vector3& vector, const Vector3& onto);

	/// <summary>
	/// ベクトル反射
	/// </summary>
	/// <param name="input">反射するベクトル</param>
	/// <param name="normal">平面の法線</param>
	/// <returns></returns>
	static Vector3 Reflect(const Vector3& input, const Vector3& normal);

	/// <summary>
	/// XYZ各要素をmin、max範囲内に収める
	/// </summary>
	/// <param name="vector">元ベクトル</param>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns></returns>
	static Vector3 Clamp(const Vector3& vector, const Vector3& min, const Vector3& max);

	/// <summary>
	/// 球面線形補間
	/// </summary>
	/// <param name="from">t=0のベクトル</param>
	/// <param name="to">t=1のベクトル</param>
	/// <param name="t">媒介変数</param>
	/// <returns></returns>
	static Vector3 Slerp(const Vector3& from, const Vector3& to, const r32& t);
};

// ------------------インライン関数定義------------------
inline constexpr Vector3::Vector3() noexcept :
	x(0.0f),
	y(0.0f),
	z(0.0f) {
}

inline constexpr Vector3::Vector3(r32 x, r32 y, r32 z) noexcept :
	x(x),
	y(y),
	z(z) {
}

inline constexpr Vector3 Vector3::operator+() const noexcept {
	return *this;
}

inline constexpr Vector3 Vector3::operator-() const noexcept {
	return { -x, -y, -z };
}

inline constexpr Vector3 Vector3::operator+(const Vector3& opr) const noexcept {
	return { x + opr.x, y + opr.y, z + opr.z };
}

inline constexpr Vector3 Vector3::operator-(const Vector3& opr) const noexcept {
	return { x - opr.x, y - opr.y, z - opr.z };
}

inline constexpr Vector3 Vector3::operator*(r32 times) const noexcept {
	return { x * times, y * times, z * times };
}

inline constexpr Vector3 Vector3::operator/(r32 times) const noexcept {
	return { x / times, y / times, z / times };
}

inline constexpr Vector3& Vector3::operator+=(const Vector3& opr) noexcept {
	x += opr.x;
	y += opr.y;
	z += opr.z;
	return *this;
}

inline constexpr Vector3& Vector3::operator-=(const Vector3& opr) noexcept {
	x -= opr.x;
	y -= opr.y;
	z -= opr.z;
	return *this;
}

inline constexpr Vector3& Vector3::operator*=(r32 times) noexcept {
	x *= times;
	y *= times;
	z *= times;
	return *this;
}

inline constexpr Vector3& Vector3::operator/=(r32 times) noexcept {
	x /= times;
	y /= times;
	z /= times;
	return *this;
}

inline constexpr bool Vector3::operator==(const Vector3& opr) const noexcept {
	return x == opr.x && y == opr.y && z == opr.z;
}

inline constexpr bool Vector3::operator!=(const Vector3& opr) const noexcept {
	return !(*this == opr);
}

inline constexpr const r32& Vector3::operator[](size_t i) const {
	return *(&x + i);
}

inline constexpr r32& Vector3::operator[](size_t i) {
	return *(&x + i);
}

constexpr r32 Vector3::Dot(const Vector3& input1, const Vector3& input2) noexcept {
	return input1.x * input2.x + input1.y * input2.y + input1.z * input2.z;
}

constexpr Vector3 Vector3::Cross(const Vector3& vector1, const Vector3& vector2) noexcept {
	return Vector3{
		vector1.y * vector2.z - vector1.z * vector2.y,
		vector1.z * vector2.x - vector1.x * vector2.z,
		vector1.x * vector2.y - vector1.y * vector2.x,
	};
}

constexpr Vector3 Vector3::Add(const Vector3& vector1, const Vector3& vector2) noexcept {
	return vector1 + vector2;
}

constexpr Vector3 Vector3::Subtract(const Vector3& vector1, const Vector3& vector2) noexcept {
	return vector1 - vector2;
}

constexpr Vector3 Vector3::Multiply(const Vector3& vector, const r32& times) noexcept {
	return vector * times;
}

constexpr Vector3 Vector3::Multiply(const Vector3& vector, const Vector3& times) noexcept {
	return Vector3{ vector.x * times.x, vector.y * times.y, vector.z * times.z };
}

constexpr Vector3 Vector3::Lerp(const Vector3& from, const Vector3& to, r32 t) noexcept {
	return from * (1 - t) + to * t;
}

inline constexpr Vector3 Vector3::LerpElement(const Vector3& from, const Vector3& to, const Vector3& t) noexcept {
	return {
		from.x * (1 - t.x) + to.x * t.x,
		from.y * (1 - t.y) + to.y * t.y,
		from.z * (1 - t.z) + to.z * t.z
	};
}

constexpr Vector3 Vector3::Bezier(const Vector3& initial, const Vector3& control, const Vector3& terminal, const r32 t) {
	return Lerp(Lerp(initial, control, t), Lerp(control, terminal, t), t);
}

/// <summary>
/// Vector3定数
/// </summary>
namespace CVector3 {

constexpr Vector3 BASIS{ 1.0f, 1.0f, 1.0f }; // x(1.0f), y(1.0f), z(1.0f)
constexpr Vector3 BASIS_X{ 1.0f, 0.0f, 0.0f }; // x(1.0f), y(0.0f), z(0.0f)
constexpr Vector3 BASIS_Y{ 0.0f, 1.0f, 0.0f }; // x(0.0f), y(1.0f), z(0.0f)
constexpr Vector3 BASIS_Z{ 0.0f, 0.0f, 1.0f }; // x(0.0f), y(0.0f), z(1.0f)
constexpr Vector3 ONE{ BASIS }; // x(0.0f), y(0.0f), z(0.0f)
constexpr Vector3 ZERO{ 0.0f, 0.0f, 0.0f }; // x(0.0f), y(0.0f), z(0.0f)
constexpr Vector3 HALF{ 0.5f, 0.5f, 0.5f }; // x(0.5f), y(0.5f), z(0.5f)
constexpr Vector3 INFINTY{ std::numeric_limits<r32>::infinity(), std::numeric_limits<r32>::infinity(), std::numeric_limits<r32>::infinity() };
constexpr Vector3 INFINTY_X{ std::numeric_limits<r32>::infinity(), 0, 0 };
constexpr Vector3 INFINTY_Y{ 0, std::numeric_limits<r32>::infinity(), 0 };
constexpr Vector3 INFINTY_Z{ 0, 0, std::numeric_limits<r32>::infinity() };

constexpr Vector3 FORWARD{ BASIS_Z };
constexpr Vector3 BACKWARD{ -FORWARD };
constexpr Vector3 UP{ BASIS_Y };
constexpr Vector3 DOWN{ -UP };
constexpr Vector3 RIGHT{ BASIS_X };
constexpr Vector3 LEFT{ -RIGHT };

}
