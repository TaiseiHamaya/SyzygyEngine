#pragma once

#include <memory>
#include <array>

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <wrl/client.h>
#include <dinput.h>
#include <Xinput.h>

#include <Library/Math/Vector2.h>

#include "./InputEnum.h"

namespace szg {

class Input {
private:
	Input() = default;
	~Input() = default;

	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

public:
	static Input& GetInstance();

public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	static void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	static void Update();

	// ----------キーボード----------

	/// <summary>
	/// キーが押されている
	/// </summary>
	/// <param name="id">ID</param>
	/// <returns></returns>
	static bool IsPressKey(KeyID id);

	// ----------マウス----------
	/// <summary>
	/// マウスボタンが押されている
	/// </summary>
	/// <param name="id">ID</param>
	/// <returns></returns>
	static bool IsPressMouse(MouseID id);

	/// <summary>
	/// ウィンドウ基準でマウス位置を取得
	/// </summary>
	/// <returns></returns>
	static const Vector2& MousePosition();

	/// <summary>
	/// マウスの移動量を取得
	/// </summary>
	/// <returns></returns>
	static Vector2 MouseDelta();

	/// <summary>
	/// マウスホイールの移動量を取得
	/// </summary>
	/// <returns>[-1,1]</returns>
	static r32 WheelDelta();

	// ----------ゲームパッド----------
	/// <summary>
	/// Padボタンが押されているか
	/// </summary>
	/// <param name="id">ID</param>
	/// <returns></returns>
	static bool IsPressPad(PadID id);

	/// <summary>
	/// 左スティックの入力
	/// </summary>
	/// <returns>[-1,1]の値</returns>
	static Vector2 StickL();

	/// <summary>
	/// 右スティックの入力
	/// </summary>
	/// <returns>[-1,1]の値</returns>
	static Vector2 StickR();

	/// <summary>
	/// 左ショルダーの入力
	/// </summary>
	/// <returns>[0,1]の値</returns>
	static r32 TriggerL();

	/// <summary>
	/// 右ショルダーの入力
	/// </summary>
	/// <returns>[0,1]の値</returns>
	static r32 TriggerR();

	/// <summary>
	/// スティックのデッドゾーン設定
	/// </summary>
	/// <param name="deadZone">[0,1]で設定</param>
	static void SetDeadZone(r32 deadZone) { GetInstance().deadZone = deadZone; };

	/// <summary>
	/// 接続されたゲームパッド数を取得
	/// </summary>
	/// <returns>最大4(XInputの制約)</returns>
	static u32 NumPad();

	/// <summary>
	/// 使用可能Padをビットフラグで取得(未デバッグ)
	/// </summary>
	/// <returns></returns>
	static u8 ActivePad();

private:
	void create_direct_input();
	void create_keyboard_device();
	void create_mouse_device();
	void initialize_joystate();

private:
	Microsoft::WRL::ComPtr<IDirectInput8> directInput = nullptr;

	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboardDevice;
	std::array<BYTE, 256> keyboardState;

	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouseDevice;
	Vector2 mousePosition;
	std::unique_ptr<DIMOUSESTATE2> mouseState;

	u32 dwUserIndex = 0;
	std::unique_ptr<XINPUT_STATE> joystate;
	r32 deadZone = 0.2f;
};

class InputAdvanced {
private:
	InputAdvanced() = default;
	~InputAdvanced() = default;

	InputAdvanced(const InputAdvanced&) = delete;
	InputAdvanced& operator=(const InputAdvanced&) = delete;

public:
	/// <summary>
	/// WASDの入力を取得
	/// ※ワールド空間ベース
	/// </summary>
	/// <returns></returns>
	static Vector2 PressWASD();

	/// <summary>
	/// 矢印の入力を取得
	/// ※ワールド空間ベース
	/// </summary>
	/// <returns></returns>
	static Vector2 PressArrow();

	/// <summary>
	/// 4キーの入力を取得
	/// </summary>
	/// <param name="up">Y+</param>
	/// <param name="down">Y-</param>
	/// <param name="left">X-</param>
	/// <param name="right">X+</param>
	/// <returns></returns>
	static Vector2 PressCustom(KeyID up, KeyID down, KeyID left, KeyID right);

private:
	static void NormalizeOneOrOver(Vector2& vector);
};

}; // szg
