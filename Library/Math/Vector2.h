#pragma once

#include <limits>

/// <summary>
/// 2次元ベクトル構造体
/// <param name="x">x方向のベクトル</param>
/// <param name="y">y方向のベクトル</param>
/// </summary>
class Vector2 final { // 2次元ベクトル
	/// <summary>
	/// コンストラクタ
	/// </summary>
public:
	inline constexpr Vector2() noexcept;
	inline constexpr Vector2(r32 x, r32 y) noexcept;
	inline constexpr Vector2(const Vector2& vec) noexcept = default;
	inline constexpr Vector2(Vector2&& vec) noexcept = default;

	~Vector2() noexcept = default;

public:
	// ------------------基礎構造------------------
	r32 x; // X座標
	r32 y; // Y座標

public:
	// ------------------operator------------------
	inline constexpr Vector2& operator=(const Vector2& vec) noexcept = default;
	inline constexpr Vector2& operator=(Vector2&& vec) noexcept = default;
	inline constexpr const Vector2 operator+(void) const noexcept;
	inline constexpr const Vector2 operator-(void) const noexcept;
	inline constexpr const Vector2 operator+(const Vector2& opr) const noexcept;
	inline constexpr const Vector2 operator-(const Vector2& opr) const noexcept;
	inline constexpr const Vector2 operator*(r32 times) const noexcept;
	inline constexpr const Vector2 operator/(r32 times) const noexcept;
	inline constexpr Vector2& operator+=(const Vector2& opr) noexcept;
	inline constexpr Vector2& operator-=(const Vector2& opr) noexcept;
	inline constexpr Vector2& operator*=(r32 times) noexcept;
	inline constexpr Vector2& operator/=(r32 times) noexcept;
	inline constexpr bool operator==(const Vector2& opr) const noexcept;
	inline constexpr bool operator!=(const Vector2& opr) const noexcept;

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
	const Vector2 normalize(void) const noexcept(false);

	/// <summary>
	/// ベクトルの正規化
	/// </summary>
	/// <param name="disapproval">範囲外だった場合の戻り値</param>
	/// <param name="tolerance">lengthの許容値</param>
	/// <returns></returns>
	const Vector2 normalize_safe(const Vector2& disapproval = { 1,0 }, r32 tolerance = 1e-4f) const noexcept;

public:
	// ------------------static関数------------------

	/// <summary>
	/// 2つのベクトルの内積を計算
	/// <param name="input1">2次元ベクトル1</param>
	/// <param name="input2">2次元ベクトル2</param>
	/// </summary>
	static constexpr r32 Dot(const Vector2& input1, const Vector2& input2) noexcept;

	/// <summary>
	/// 2つのベクトルの外積を計算
	/// <param name="input1">2次元ベクトル1</param>
	/// <param name="input2">2次元ベクトル2</param>
	/// </summary>
	static constexpr r32 Cross(const Vector2& input1, const Vector2& input2) noexcept;

	/// <summary>
	/// ベクトルの大きさを計算
	/// <param name="vector">終点の座標</param>
	/// </summary>
	static r32 Length(const Vector2& vector) noexcept;

	/// <summary>
	/// ベクトルの大きさを計算
	/// <param name="vector1">終点の座標</param>
	/// <param name="vector2">始点の座標</param>
	/// </summary>
	static r32 Length(const Vector2& vector1, const Vector2& vector2) noexcept;

	/// <summary>
	/// ベクトルの正規化
	/// <param name="vector1">終点の座標</param>
	/// </summary>
	static const Vector2 Normalize(const Vector2& vector);

	/// <summary>
	/// ベクトルの正規化
	/// <param name="vector1">始点の座標</param>
	/// <param name="vector2">終点の座標</param>
	/// </summary>
	static const Vector2 Normalize(const Vector2& vectorFrom, const Vector2& vectorTo);

	/// <summary>
	/// 2つのベクトルを加算
	/// <param name="vector1">足されるベクトル</param>
	/// <param name="vector2">足すベクトル</param>
	/// </summary>
	static constexpr const Vector2 Add(const Vector2& vector1, const Vector2& vector2) noexcept;

	/// <summary>
	/// 2つのベクトルを減算
	/// <param name="vector1">引かれるベクトル</param>
	/// <param name="vector2">引くベクトル</param>
	/// </summary>
	static constexpr const Vector2 Subtract(const Vector2& vector1, const Vector2& vector2) noexcept;

	/// <summary>
	/// ベクトルを定数倍
	/// <param name="vector1">乗算されるベクトル</param>
	/// <param name="times">係数</param>
	/// </summary>
	static constexpr const Vector2 Multiply(const Vector2& vector, const r32& times) noexcept;

	/// <summary>
	/// ベクトルを定数倍
	/// <param name="vector1">乗算されるベクトル</param>
	/// <param name="times">係数</param>
	/// </summary>
	static constexpr const Vector2 Multiply(const Vector2& vector, const Vector2& times) noexcept;

	/// <summary>
	/// 2つの位置ベクトルの間の座標
	/// <param name="from">始点の位置ベクトル</param>
	/// <param name="to">終点の位置ベクトル</param>
	/// <param name="t">割合T</param>
	/// </summary>
	static constexpr const Vector2 Lerp(const Vector2& from, const Vector2& to, const r32& t) noexcept;

	/// <summary>
	/// <param name="vector">変換するベクトル</param>
	/// <param name="radian">回転量(ラジアン)</param>
	/// </summary>
	static const Vector2 Rotate(const Vector2& vector, const r32& radian) noexcept;

	/// <summary>
	/// <param name="vector">変換するベクトル</param>
	/// <param name="radian">回転量(ラジアン)</param>
	/// </summary>
	static constexpr const Vector2 Rotate(const Vector2& vector, const r32 sinTheta, const r32 cosTheta) noexcept;

	/// <summary>
	/// 2次ベジエ曲線
	/// </summary>
	/// <param name="initial">始点</param>
	/// <param name="control">制御点</param>
	/// <param name="terminal">終点</param>
	/// <param name="t">媒介変数</param>
	/// <returns></returns>
	static constexpr const Vector2 Bezier(const Vector2& initial, const Vector2& control, const Vector2& terminal, const r32 t) noexcept;

	/// <summary>
	/// ベクトルの各要素の絶対値化
	/// </summary>
	/// <param name="vector">変化させるベクトル</param>
	/// <returns></returns>
	static const Vector2 Abs(const Vector2& vector) noexcept;

	/// <summary>
	/// ベクトル射影
	/// </summary>
	/// <param name="vector">射影するベクトル</param>
	/// <param name="onto">射影対象の単位ベクトル</param>
	/// <returns></returns>
	static Vector2 Projection(const Vector2& vector, const Vector2& onto);

	/// <summary>
	/// ベクトル反射
	/// </summary>
	/// <param name="input">反射するベクトル</param>
	/// <param name="normal">平面の法線</param>
	/// <returns></returns>
	static Vector2 Reflect(const Vector2& input, const Vector2& normal);

	/// <summary>
	/// XYZ各要素をmin、max範囲内に収める
	/// </summary>
	/// <param name="vector">元ベクトル</param>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns></returns>
	static Vector2 Clamp(const Vector2& vector, const Vector2& min, const Vector2& max);

	/// <summary>
	/// 球面線形補間
	/// </summary>
	/// <param name="from">t=0のベクトル</param>
	/// <param name="to">t=1のベクトル</param>
	/// <param name="t">媒介変数</param>
	/// <returns></returns>
	static Vector2 Slerp(const Vector2& from, const Vector2& to, const r32& t);

};

inline constexpr Vector2::Vector2() noexcept :
	x(0.0f), y(0.0f) {
}

inline constexpr Vector2::Vector2(r32 x, r32 y) noexcept :
	x(x), y(y) {
}

// ------------------インライン関数定義------------------

inline constexpr const Vector2 Vector2::operator+() const noexcept {
	return *this;
}

inline constexpr const Vector2 Vector2::operator-() const noexcept {
	return { -x, -y };
}

inline constexpr const Vector2 Vector2::operator+(const Vector2& opr) const noexcept {
	return { x + opr.x, y + opr.y };
}

inline constexpr const Vector2 Vector2::operator-(const Vector2& opr) const noexcept {
	return { x - opr.x, y - opr.y };
}

inline constexpr const Vector2 Vector2::operator*(r32 times) const noexcept {
	return { x * times, y * times };
}

inline constexpr const Vector2 Vector2::operator/(r32 times) const noexcept {
	return { x / times, y / times };
}

inline constexpr Vector2& Vector2::operator+=(const Vector2& opr) noexcept {
	x += opr.x;
	y += opr.y;
	return *this;
}

inline constexpr Vector2& Vector2::operator-=(const Vector2& opr) noexcept {
	x -= opr.x;
	y -= opr.y;
	return *this;
}

inline constexpr Vector2& Vector2::operator*=(r32 times) noexcept {
	x *= times;
	y *= times;
	return *this;
}

inline constexpr Vector2& Vector2::operator/=(r32 times) noexcept {
	x /= times;
	y /= times;
	return *this;
}

inline constexpr bool Vector2::operator==(const Vector2& opr) const noexcept {
	return x == opr.x && y == opr.y;
}

inline constexpr bool Vector2::operator!=(const Vector2& opr) const noexcept {
	return !(*this == opr);
}

constexpr r32 Vector2::Dot(const Vector2& input1, const Vector2& input2) noexcept {
	return input1.x * input2.x + input1.y * input2.y;
}

constexpr r32 Vector2::Cross(const Vector2& input1, const Vector2& input2) noexcept {
	return input1.x * input2.y - input1.y * input2.x;
}

constexpr const Vector2 Vector2::Add(const Vector2& vector1, const Vector2& vector2) noexcept {
	return vector1 + vector2;
}

constexpr const Vector2 Vector2::Subtract(const Vector2& vector1, const Vector2& vector2) noexcept {
	return vector1 - vector2;
}

constexpr const Vector2 Vector2::Multiply(const Vector2& vector, const r32& times) noexcept {
	return Vector2{ vector.x * times, vector.y * times };
}

constexpr const Vector2 Vector2::Multiply(const Vector2& vector, const Vector2& times) noexcept {
	return Vector2{ vector.x * times.x, vector.y * times.y };
}

constexpr const Vector2 Vector2::Lerp(const Vector2& from, const Vector2& to, const r32& t) noexcept {
	return from * (1 - t) + to * t;
}

constexpr const Vector2 Vector2::Bezier(const Vector2& initial, const Vector2& control, const Vector2& terminal, const r32 t) noexcept {
	return Lerp(Lerp(initial, control, t), Lerp(control, terminal, t), t);
}

constexpr const Vector2 Vector2::Rotate(const Vector2& vector, const r32 sinTheta, const r32 cosTheta) noexcept {
	return Vector2{
		vector.x * cosTheta - vector.y * sinTheta,
		vector.x * sinTheta + vector.y * cosTheta };
}

/// <summary>
/// Vector2定数
/// </summary>
namespace CVector2 {

constexpr Vector2 BASIS_X{ 1.0f, 0.0f }; // x(1.0f), y(0.0f)
constexpr Vector2 BASIS_Y{ 0.0f, 1.0f }; // x(0.0f), y(1.0f)
constexpr Vector2 BASIS{ 1.0f, 1.0f }; // x(1.0f), y(1.0f)
constexpr Vector2 ONE{ BASIS }; // x(0.0f), y(0.0f)
constexpr Vector2 ZERO{ 0.0f, 0.0f }; // x(0.0f), y(0.0f)
constexpr Vector2 HALF{ 0.5f, 0.5f }; // x(0.5f), y(0.5f)
constexpr Vector2 INFINTY{ std::numeric_limits<r32>::infinity(),std::numeric_limits<r32>::infinity() };
constexpr Vector2 INFINTY_X{ std::numeric_limits<r32>::infinity(),0 };
constexpr Vector2 INFINTY_Y{ 0, std::numeric_limits<r32>::infinity() };

constexpr Vector2 FORWARD{ BASIS_X };
constexpr Vector2 BACKWARD{ -FORWARD };
constexpr Vector2 RIGHT{ FORWARD };
constexpr Vector2 LEFT{ -RIGHT };
constexpr Vector2 UP{ BASIS_Y };
constexpr Vector2 DOWN{ -UP };

};
