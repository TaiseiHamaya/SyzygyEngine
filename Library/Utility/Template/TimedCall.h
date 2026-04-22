#pragma once

#include <functional>

#include "Engine/Runtime/Clock/WorldTimer.h"

template<class Type>
class TimedCall {
public:
	TimedCall() = default;
	TimedCall(std::function<Type>&& function_, r32 time_, bool isFinished = false);
	~TimedCall() = default;

	void update();

	bool is_finished() const;

	void restart(r32 time);

	void end_force();

#ifdef DEBUG_FEATURES_ENABLE
public:
	void debug_gui();
#endif // _DEBUG

private:
	std::function<Type> function;
	szg::WorldTimer timer;
	bool isFinished;
};

template<class Type>
inline TimedCall<Type>::TimedCall(std::function<Type>&& function_, r32 time_, bool isFinished_) :
	isFinished(isFinished_) {
	function = function_;
	timer.set(time_);
}

template<class Type>
inline void TimedCall<Type>::update() {
	timer.back();
	if (timer <= 0) {
		isFinished = true;
		function();
	}
}

template<class Type>
inline bool TimedCall<Type>::is_finished() const {
	return isFinished;
}

template<class Type>
inline void TimedCall<Type>::restart(r32 time_) {
	timer.set(time_);
	isFinished = false;
}

template<class Type>
inline void TimedCall<Type>::end_force() {
	timer.set(0);
}

#ifdef DEBUG_FEATURES_ENABLE

#include <imgui.h>
#include <format>

template<class Type>
inline void TimedCall<Type>::debug_gui() {
	if (ImGui::CollapsingHeader(std::format("TimedCall##{}", (void*)this).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text(std::format("Finished : {:s}", isFinished).c_str());
		ImGui::Text(std::format("Timer : {}", timer.time()).c_str());
	}
}
#endif // _DEBUG
