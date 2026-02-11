#include "NodeAnimationPlayer.h"

using namespace szg;

#include <cmath>

#include "NodeAnimationAsset.h"
#include "NodeAnimationLibrary.h"

template<typename T, T LerpFunction(const T&, const T&, r32) = std::lerp>
T CalculateValue(const NodeAnimationAsset::AnimationCurve<T>& animationCurve, r32 time);

NodeAnimationPlayer::NodeAnimationPlayer(const std::string&, const std::string& animationName_, bool isLoop_) :
	isLoop(isLoop_) {
	reset_animation(animationName_);
}

void NodeAnimationPlayer::update() noexcept {
	if (!isActive || !nodeAnimation) {
		return;
	}
	timer.ahead(animationSpeed);
	if (isLoop && is_end()) {
		timer.set(std::fmod(timer, nodeAnimation->duration()));
	}
}

Vector3 NodeAnimationPlayer::calculate_scale(const std::string& nodeName) const {
	if (!nodeAnimation || !nodeAnimation->contains(nodeName)) {
		return CVector3::BASIS;
	}
	return CalculateValue<Vector3, Vector3::Lerp>(nodeAnimation->node(nodeName).scale, timer);
}

Quaternion NodeAnimationPlayer::calculate_rotate(const std::string& nodeName) const {
	if (!nodeAnimation || !nodeAnimation->contains(nodeName)) {
		return CQuaternion::IDENTITY;
	}
	return CalculateValue<Quaternion, Quaternion::Slerp>(nodeAnimation->node(nodeName).rotate, timer);
}

Vector3 NodeAnimationPlayer::calculate_translate(const std::string& nodeName) const {
	if (!nodeAnimation || !nodeAnimation->contains(nodeName)) {
		return CVector3::ZERO;
	}
	return CalculateValue<Vector3, Vector3::Lerp>(nodeAnimation->node(nodeName).translate, timer);
}

void NodeAnimationPlayer::reset_animation(const std::string& animationName_) {
#ifdef DEBUG_FEATURES_ENABLE
	animationName = animationName_.empty() ? "UnknownAnimation" : animationName_;
#endif // _DEBUG

	nodeAnimation = NodeAnimationLibrary::GetAnimation(animationName_);
}

void NodeAnimationPlayer::play() noexcept {
	isActive = true;
}

void NodeAnimationPlayer::stop() noexcept {
	isActive = false;
	timer.set(0);
}

void NodeAnimationPlayer::pause() noexcept {
	isActive = false;
}

void NodeAnimationPlayer::restart() noexcept {
	stop();
	play();
}

bool NodeAnimationPlayer::is_end() const noexcept {
	return parametric() >= 1.0f;
}

r32 NodeAnimationPlayer::parametric() const noexcept {
	if (nodeAnimation) {
		return timer / nodeAnimation->duration();
	}
	return 1.0f;
}

void NodeAnimationPlayer::set_loop(bool isLoop_) noexcept {
	isLoop = isLoop_;
}

void NodeAnimationPlayer::set_time_force(r32 timer_) noexcept {
	timer.set(timer_);
}

void NodeAnimationPlayer::animation_speed(r32 speed) noexcept {
	animationSpeed = speed;
}

template<typename T, T LerpFunction(const T&, const T&, r32)>
T CalculateValue(const NodeAnimationAsset::AnimationCurve<T>& animationCurve, r32 time) {
	const std::map<r32, T>& keyframes = animationCurve.keyframes;
	// キーフレームがない場合はTを初期値で返す
	if (keyframes.empty()) {
		return T{};
	}
	// 要素が1もしくは先頭のキーフレームより早い場合は先頭のValueを返す
	if (keyframes.size() == 1 || time <= keyframes.begin()->first) {
		return keyframes.begin()->second;
	}

	// にぶたんしてKeyを探す
	auto endKey = keyframes.upper_bound(time);
	// 末尾より後ろの場合は末尾の値を返す
	if (endKey == keyframes.end()) {
		return keyframes.rbegin()->second;
	}
	// 1つ前のKeyframeを取得
	auto beginKey = std::prev(endKey);
	r32 parametric = (time - beginKey->first) / (endKey->first - beginKey->first);
	return LerpFunction(beginKey->second, endKey->second, parametric);
}
