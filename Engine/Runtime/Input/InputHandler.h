#pragma once

#include <bitset>
#include <unordered_map>
#include <vector>

#include "Engine/Runtime/Clock/WorldTimer.h"
#include "Engine/Runtime/Input/Input.h"

namespace szg {

/// <summary>
/// InputHandler 初期化時の履歴設定モード
/// </summary>
enum class InputInitializeMode {
	Default,  // 履歴を初期化しない
	True,     // 押下状態として履歴を初期化
	False,    // 離れ状態として履歴を初期化
	Current,  // 現在の入力状態と同じ状態として履歴を初期化
	Inverted, // 現在の入力状態の反転として履歴を初期化
};

template<typename T>
struct InputHandlerFunction;

template<>
struct InputHandlerFunction<KeyID> {
	inline bool operator()(KeyID id) {
		return Input::IsPressKey(id);
	}
};

template<>
struct InputHandlerFunction<PadID> {
	inline bool operator()(PadID id) {
		return Input::IsPressPad(id);
	};
};

template<>
struct InputHandlerFunction<MouseID> {
	inline bool operator()(MouseID id) {
		return Input::IsPressMouse(id);
	};
};

template<typename T, class InputFunction = InputHandlerFunction<T>>
class InputHandler {
private:
	struct Data {
		WorldTimer timer;
		std::bitset<2> flags;
	};

public:
	InputHandler() = default;
	~InputHandler() = default;

	InputHandler(std::vector<T> keys);

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="keys">検知するキー</param>
	/// <param name="mode">初期化時の履歴設定モード</param>
	void initialize(std::vector<T> keys, InputInitializeMode mode = InputInitializeMode::Default);

	/// <summary>
	/// 更新処理
	/// </summary>
	void update();

	bool trigger(T id);
	bool press(T id);
	bool release(T id);
	bool idle(T id);

	/// <summary>
	/// 押されている時間
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	r32 press_timer(T id);

	/// <summary>
	/// 離れている時間
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	r32 idle_timer(T id);

private:
	InputFunction inputter{};
	std::unordered_map<T, Data> data;
};

template<typename T, class InputFunction>
inline InputHandler<T, InputFunction>::InputHandler(std::vector<T> keys) {
	initialize(keys);
}

template<typename T, class InputFunction>
inline void InputHandler<T, InputFunction>::initialize(std::vector<T> keys, InputInitializeMode mode) {
	data.clear();
	data.reserve(keys.size());
	for (T& key : keys) {
		std::bitset<2> initialState{ 0b00 };
		switch (mode) {
		case InputInitializeMode::True:
			initialState.set(0, true);
			break;
		case InputInitializeMode::False:
			initialState.set(0, false);
			break;
		case InputInitializeMode::Current:
			initialState.set(0, inputter(key));
			break;
		case InputInitializeMode::Inverted:
			initialState.set(0, !inputter(key));
			break;
		case InputInitializeMode::Default:
		default:
			continue;
		}
		data.try_emplace(key, WorldTimer{}, initialState);
	}
}

template<typename T, class InputFunction>
inline void InputHandler<T, InputFunction>::update() {
	for (auto& [id, elem] : data) {
		elem.flags <<= 1;
		elem.flags.set(0, inputter(id));
		if (elem.flags.count() == 1) {
			elem.timer.set(0);
		}
		else {
			elem.timer.ahead();
		}
	}
}

template<typename T, class InputFunction>
inline bool InputHandler<T, InputFunction>::trigger(T id) {
	if (!data.contains(id)) {
		return false;
	}
	return data[id].flags == std::bitset<2>(0b01);
}

template<typename T, class InputFunction>
inline bool InputHandler<T, InputFunction>::press(T id) {
	if (!data.contains(id)) {
		return false;
	}
	return data[id].flags.test(0);
}

template<typename T, class InputFunction>
inline bool InputHandler<T, InputFunction>::release(T id) {
	if (!data.contains(id)) {
		return false;
	}
	return data[id].flags == std::bitset<2>(0b10);
}

template<typename T, class InputFunction>
inline bool InputHandler<T, InputFunction>::idle(T id) {
	return !press(id);
}

template<typename T, class InputFunction>
inline r32 InputHandler<T, InputFunction>::press_timer(T id) {
	if (press(id)) {
		return data[id].timer;
	}
	return 0;
}

template<typename T, class InputFunction>
inline r32 InputHandler<T, InputFunction>::idle_timer(T id) {
	if (idle(id)) {
		return data[id].timer;
	}
	return 0;
}

}; // szg
