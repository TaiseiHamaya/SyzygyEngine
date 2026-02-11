#pragma once

#include <memory>
#include <string>

#include "./NodeAnimationAsset.h"

#include <Library/Utility/Tools/ConstructorMacro.h>
#include <Library/Math/Quaternion.h>
#include <Library/Math/Vector3.h>

#include "Engine/Runtime/Clock/WorldTimer.h"

namespace szg {

class NodeAnimationPlayer {
public:
	NodeAnimationPlayer() noexcept = default;
	NodeAnimationPlayer(const std::string& fileName, const std::string& animationName, bool isLoop);
	~NodeAnimationPlayer() = default;

	SZG_CLASS_MOVE_ONLY(NodeAnimationPlayer)

public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void update() noexcept;

	Vector3 calculate_scale(const std::string& nodeName) const;
	Quaternion calculate_rotate(const std::string& nodeName) const;
	Vector3 calculate_translate(const std::string& nodeName) const;

	void reset_animation(const std::string& animationName);

	void play() noexcept;
	void stop() noexcept;
	void pause() noexcept;
	void restart() noexcept;

	bool is_end() const noexcept;

	r32 parametric() const noexcept;

	void set_loop(bool isLoop) noexcept;
	
	void set_time_force(r32 timer) noexcept;
	
	void animation_speed(r32 speed = 1.0f) noexcept;

private:
	bool isLoop{ false };
	bool isActive{ true };
	WorldTimer timer;
	r32 animationSpeed{ 1.0f };
	std::shared_ptr<const NodeAnimationAsset> nodeAnimation;

#ifdef DEBUG_FEATURES_ENABLE
	std::string animationName;
#endif // _DEBUG
};

}; // szg
