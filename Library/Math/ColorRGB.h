#pragma once

class ColorRGB final {
public:
	/// <summary>
	/// デフォルトコンストラクタ(1,1,1,1で初期化)
	/// </summary>
	constexpr ColorRGB() noexcept = default;

	/// <summary>
	/// カラーコードから生成
	/// </summary>
	/// <param name="hex">カラーコード</param>
	constexpr ColorRGB(u32 hex) noexcept;

	/// <summary>
	/// RGB別カラーコードから生成
	/// </summary>
	/// <param name="_red">赤[0x00, 0xff]</param>
	/// <param name="_green">緑[0x00, 0xff]</param>
	/// <param name="_blue">青[0x00, 0xff]</param>
	/// <param name="_alpha">アルファ[0x00, 0xff]</param>
	constexpr ColorRGB(u8 _red, u8 _green, u8 _blue) noexcept;

	/// <summary>
	/// [0,1]のカラーデータから生成
	/// </summary>
	/// <param name="_red">赤[0, 1]</param>
	/// <param name="_green">緑[0, 1]</param>
	/// <param name="_blue">青[0, 1]</param>
	/// <param name="_alpha">アルファ[0, 1]</param>
	constexpr ColorRGB(r32 _red, r32 _green, r32 _blue) noexcept;

public:
	r32 red{ 1.0f }; // 赤[0, 1]
	r32 green{ 1.0f }; // 緑[0, 1]
	r32 blue{ 1.0f }; // 青[0, 1]

#ifdef DEBUG_FEATURES_ENABLE
public:
	void debug_gui(string_literal tag = "ColorRGB") noexcept(false);
#endif // _DEBUG
};

constexpr ColorRGB::ColorRGB(u32 hex) noexcept :
	red(((hex >> 24) & 0x000000ff) / 255.0f),
	green(((hex >> 16) & 0x000000ff) / 255.0f),
	blue(((hex >> 8) & 0x000000ff) / 255.0f) {
}

constexpr ColorRGB::ColorRGB(u8 _red, u8 _green, u8 _blue) noexcept :
	red(_red / 255.0f),
	green(_green / 255.0f),
	blue(_blue / 255.0f) {
}

constexpr ColorRGB::ColorRGB(r32 _red, r32 _green, r32 _blue) noexcept :
	red(_red),
	green(_green),
	blue(_blue) {
}

/// <summary>
/// Color定数
/// </summary>
namespace CColorRGB {

constexpr ColorRGB WHITE{ 1.0f, 1.0f, 1.0f };
constexpr ColorRGB BLACK{ 0.0f, 0.0f, 0.0f };
constexpr ColorRGB RED{ 1.0f, 0.0f, 0.0f };
constexpr ColorRGB GREEN{ 0.0f, 1.0f, 0.0f };
constexpr ColorRGB BLUE{ 0.0f, 0.0f, 1.0f };

constexpr ColorRGB YELLOW{ 1.0f, 1.0f, 0.0f };
constexpr ColorRGB MAGENTA{ 1.0f, 0.0f, 1.0f };
constexpr ColorRGB CYAN{ 0.0f, 1.0f, 1.0f };

constexpr ColorRGB ZERO{ 0.0f, 0.0f, 0.0f };
constexpr ColorRGB ZERO_WHITE{ 1.0f, 1.0f, 1.0f };

}


