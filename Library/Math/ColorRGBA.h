#pragma once

class ColorRGBA final {
public:
	/// <summary>
	/// デフォルトコンストラクタ(1,1,1,1で初期化)
	/// </summary>
	constexpr ColorRGBA() noexcept = default;

	/// <summary>
	/// カラーコードから生成
	/// </summary>
	/// <param name="hex">カラーコード</param>
	constexpr ColorRGBA(u32 hex) noexcept;

	/// <summary>
	/// RGB別カラーコードから生成
	/// </summary>
	/// <param name="_red">赤[0x00, 0xff]</param>
	/// <param name="_green">緑[0x00, 0xff]</param>
	/// <param name="_blue">青[0x00, 0xff]</param>
	/// <param name="_alpha">アルファ[0x00, 0xff]</param>
	constexpr ColorRGBA(u8 _red, u8 _green, u8 _blue, u8 _alpha) noexcept;

	/// <summary>
	/// [0,1]のカラーデータから生成
	/// </summary>
	/// <param name="_red">赤[0, 1]</param>
	/// <param name="_green">緑[0, 1]</param>
	/// <param name="_blue">青[0, 1]</param>
	/// <param name="_alpha">アルファ[0, 1]</param>
	constexpr ColorRGBA(r32 _red, r32 _green, r32 _blue, r32 _alpha) noexcept;

public:
	r32 red{ 1.0f }; // 赤[0, 1]
	r32 green{ 1.0f }; // 緑[0, 1]
	r32 blue{ 1.0f }; // 青[0, 1]
	r32 alpha{ 1.0f }; // アルファ[0, 1]

#ifdef DEBUG_FEATURES_ENABLE
public:
	void debug_gui(string_literal tag = "ColorRGBA") noexcept(false);
#endif // _DEBUG

public:
	/// <summary>
	/// 色の線形補間
	/// </summary>
	/// <param name="color1">t = 0の時の色</param>
	/// <param name="color2">t = 1の時の色</param>
	/// <param name="t">媒介変数</param>
	/// <returns></returns>
	static ColorRGBA Lerp(const ColorRGBA& color1, const ColorRGBA& color2, r32 t);

	static ColorRGBA LerpElement(const ColorRGBA& color1, const ColorRGBA& color2, const ColorRGBA& t);
};

constexpr ColorRGBA::ColorRGBA(u32 hex) noexcept :
	red(((hex >> 24) & 0x000000ff) / 255.0f),
	green(((hex >> 16) & 0x000000ff) / 255.0f),
	blue(((hex >> 8) & 0x000000ff) / 255.0f),
	alpha(((hex >> 0) & 0x000000ff) / 255.0f) {
}

constexpr ColorRGBA::ColorRGBA(u8 _red, u8 _green, u8 _blue, u8 _alpha) noexcept :
	red(_red / 255.0f),
	green(_green / 255.0f),
	blue(_blue / 255.0f),
	alpha(_alpha / 255.0f) {
}

constexpr ColorRGBA::ColorRGBA(r32 _red, r32 _green, r32 _blue, r32 _alpha) noexcept :
	red(_red),
	green(_green),
	blue(_blue),
	alpha(_alpha) {
}

/// <summary>
/// Color定数
/// </summary>
namespace CColorRGBA {

constexpr ColorRGBA WHITE{ 1.0f, 1.0f, 1.0f, 1.0f };
constexpr ColorRGBA BLACK{ 0.0f, 0.0f, 0.0f, 1.0f };
constexpr ColorRGBA RED{ 1.0f, 0.0f, 0.0f, 1.0f };
constexpr ColorRGBA GREEN{ 0.0f, 1.0f, 0.0f, 1.0f };
constexpr ColorRGBA BLUE{ 0.0f, 0.0f, 1.0f, 1.0f };

constexpr ColorRGBA YELLOW{ 1.0f, 1.0f, 0.0f, 1.0f };
constexpr ColorRGBA MAGENTA{ 1.0f, 0.0f, 1.0f, 1.0f };
constexpr ColorRGBA CYAN{ 0.0f, 1.0f, 1.0f, 1.0f };

constexpr ColorRGBA ZERO{ 0.0f, 0.0f, 0.0f, 0.0f };
constexpr ColorRGBA ZERO_WHITE{ 1.0f, 1.0f, 1.0f, 0.0f };

}
