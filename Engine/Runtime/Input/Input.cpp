#include "Input.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment (lib, "xinput.lib")

#include "Engine/Application/Logger.h"
#include "Engine/Application/WinApp.h"

using namespace szg;

Input& Input::GetInstance() {
	static Input instance;
	return instance;
}

void Input::Initialize() {
	Input& instance = GetInstance();

	//各種初期化
	instance.create_direct_input();
	instance.create_keyboard_device();
	instance.create_mouse_device();
	instance.initialize_joystate();
}

void Input::Update() {
	HRESULT result;
	Input& instance = GetInstance();

	// ゲームパッド更新
	XInputGetState(instance.dwUserIndex, instance.joystate.get());

	// キーボード更新
	instance.keyboardDevice->Acquire();
	result = instance.keyboardDevice->GetDeviceState(
		static_cast<DWORD>(instance.keyboardState.size() * sizeof(BYTE)),
		instance.keyboardState.data()
	);
	if (FAILED(result)) {
		// 失敗したら0埋め
		std::fill(instance.keyboardState.begin(), instance.keyboardState.end(), (BYTE)0);
	}

	// マウス更新
	instance.mouseDevice->Acquire();
	result = instance.mouseDevice->GetDeviceState(
		sizeof(DIMOUSESTATE2),
		instance.mouseState.get()
	);
	if (FAILED(result)) {
		// 失敗したら0埋め
		*instance.mouseState = { 0 };
	}

	// マウス位置更新
	POINT point;
	DWORD error;
	if (!GetCursorPos(&point)) {
		error = GetLastError();
	}
	if (!ScreenToClient(WinApp::GetWndHandle(), &point)) {
		error = GetLastError();
	}
	instance.mousePosition = { static_cast<r32>(point.x), static_cast<r32>(point.y) };
}

bool Input::IsPressKey(KeyID id) {
	i32 idInt = static_cast<i32>(id);
	auto& keyboardState = GetInstance().keyboardState;
	return keyboardState[idInt];
}

bool Input::IsPressMouse(MouseID id) {
	i32 idInt = static_cast<i32>(id);
	auto& rgbButtons = GetInstance().mouseState->rgbButtons;
	return rgbButtons[idInt];
}

const Vector2& Input::MousePosition() {
	return GetInstance().mousePosition;
}

Vector2 Input::MouseDelta() {
	Input& instance = GetInstance();
	return Vector2{
		static_cast<r32>(instance.mouseState->lX),
		static_cast<r32>(instance.mouseState->lY),
	};
}

r32 Input::WheelDelta() {
	Input& instance = GetInstance();
	// mouseLZは120区切りっぽい？
	return (instance.mouseState->lZ) / 120.0f;
}

bool Input::IsPressPad(PadID id) {
	i32 idInt = static_cast<i32>(id);
	auto& buttons = GetInstance().joystate->Gamepad.wButtons;
	return buttons & idInt;
}

Vector2 Input::StickL() {
	Input& instance = GetInstance();
	auto& gamepad = instance.joystate->Gamepad;
	Vector2 result{
		static_cast<r32>(gamepad.sThumbLX),
		static_cast<r32>(gamepad.sThumbLY)
	};
	// ShortMaxで割ることで[-1,1]にする
	result /= std::numeric_limits<SHORT>::max();

	// lengthとデッドゾーンを比較する
	if (result.length() < instance.deadZone) {
		result = CVector2::ZERO;
	}

	return result;
}

Vector2 Input::StickR() {
	Input& instance = GetInstance();
	auto& gamepad = instance.joystate->Gamepad;
	Vector2 result{
		static_cast<r32>(gamepad.sThumbRX),
		static_cast<r32>(gamepad.sThumbRY)
	};
	// ShortMaxで割ることで[-1,1]にする
	result /= std::numeric_limits<SHORT>::max();

	// lengthとデッドゾーンを比較する
	if (result.length() < instance.deadZone) {
		result = CVector2::ZERO;
	}

	return result;
}

r32 Input::TriggerL() {
	Input& instance = GetInstance();
	auto& gamepad = instance.joystate->Gamepad;
	// ByteMaxで割ることで[0,1]にする
	return (r32)gamepad.bLeftTrigger / std::numeric_limits<BYTE>::max();
}

r32 Input::TriggerR() {
	Input& instance = GetInstance();
	auto& gamepad = instance.joystate->Gamepad;
	return (r32)gamepad.bRightTrigger / std::numeric_limits<BYTE>::max();
}

u32 Input::NumPad() {
	XINPUT_STATE tester{ 0 };
	HRESULT result{};
	u32 numPad = 0;
	// 接続可能なPad数は4つまで
	for (u32 i = 0; i < 4; ++i) {
		result = XInputGetState(i, &tester);
		// S_OKだったら使用可能
		if (result == S_OK) {
			++numPad;
		}
	}
	return numPad;
}

u8 Input::ActivePad() {
	XINPUT_STATE tester{ 0 };
	HRESULT result{};
	u8 active = 0;
	// 接続可能なPad数は4つまで
	for (i32 i = 0; i < 4; ++i) {
		result = XInputGetState(i, &tester);
		// S_OKだったら使用可能
		if (result == S_OK) {
			active |= 1 << i;
		}
	}
	return active;
}

void Input::create_direct_input() {
	HRESULT result;

	// 作成
	result = DirectInput8Create(
		WinApp::GetInstanceHandle(),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(directInput.GetAddressOf()),
		nullptr
	);
	szgErrorIf(FAILED(result), "Failed initialize direct input.");
}

void Input::create_keyboard_device() {
	HRESULT result;
	// 作成
	result = directInput->CreateDevice(
		GUID_SysKeyboard,
		keyboardDevice.GetAddressOf(),
		nullptr
	);
	szgErrorIf(FAILED(result), "Failed initialize direct input.");
	// フォーマットの設定
	result = keyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	szgErrorIf(FAILED(result), "Failed initialize direct input.");
	// その他コンフィグ
	result = keyboardDevice->SetCooperativeLevel(
		WinApp::GetWndHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
	);
	szgErrorIf(FAILED(result), "Failed initialize direct input.");
}

void Input::create_mouse_device() {
	HRESULT result;
	// 作成
	result = directInput->CreateDevice(
		GUID_SysMouse,
		mouseDevice.GetAddressOf(),
		nullptr
	);
	szgErrorIf(FAILED(result), "Failed initialize direct input.");
	// フォーマットの設定
	result = mouseDevice->SetDataFormat(&c_dfDIMouse2);
	szgErrorIf(FAILED(result), "Failed initialize direct input.");
	// その他コンフィグ
	result = mouseDevice->SetCooperativeLevel(
		WinApp::GetWndHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
	);
	szgErrorIf(FAILED(result), "Failed initialize direct input.");

	// stateの生成
	mouseState = std::make_unique<DIMOUSESTATE2>();
}

void Input::initialize_joystate() {
	// stateの生成
	joystate = std::make_unique<XINPUT_STATE>();
}

Vector2 InputAdvanced::PressWASD() {
	return PressCustom(KeyID::W, KeyID::S, KeyID::A, KeyID::D);
}

Vector2 InputAdvanced::PressArrow() {
	return PressCustom(KeyID::Up, KeyID::Down, KeyID::Left, KeyID::Right);
}

Vector2 InputAdvanced::PressCustom(KeyID up, KeyID down, KeyID left, KeyID right) {
	Vector2 result = CVector2::ZERO;
	if (Input::IsPressKey(up)) {
		result.y += 1.0f;
	}
	if (Input::IsPressKey(down)) {
		result.y -= 1.0f;
	}
	if (Input::IsPressKey(right)) {
		result.x += 1.0f;
	}
	if (Input::IsPressKey(left)) {
		result.x -= 1.0f;
	}
	NormalizeOneOrOver(result);
	return result;
}

void InputAdvanced::NormalizeOneOrOver(Vector2& vector) {
	if (vector.length() >= 1) {
		vector = vector.normalize();
	}
}
