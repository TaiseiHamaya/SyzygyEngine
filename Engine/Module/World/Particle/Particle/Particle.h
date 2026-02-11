#pragma once

#include "Engine/Module/World/WorldInstance/WorldInstance.h"

#include <variant>

#include <Library/Math/ColorRGBA.h>
#include <Library/Math/Transform2D.h>

#include "Engine/Runtime/Clock/WorldTimer.h"

namespace szg {

class Particle final : public WorldInstance {
public:
	enum class RotationType {
		Constant,
		Velocity,
		LookAt,
		LookAtAngle,
		Random,
	};

	struct Constant {
		Quaternion value;
	};
	struct Random {
		Vector3 axis;
		r32 angle;
	};
	struct LookAtAngle {
		r32 angleParSec;
	};

public: // Constructor/Destructor
	Particle(
		const Vector3& translate,
		r32 lifetime_,
		const Vector3& velocity_, const Vector3& acceleration_,
		const ColorRGBA& startColor_, const ColorRGBA& endColor_,
		const Vector3& startSize_, const Vector3& endSize_,
		RotationType rotationType, std::variant<Constant, std::monostate, Random, LookAtAngle> rotationData
	);
	~Particle() = default;

	Particle(const Particle&) = delete;
	Particle& operator=(const Particle&) = delete;
	Particle(Particle&&) = delete;
	Particle& operator=(Particle&&) = delete;

public: // Member function
	void update() override;

public: // Getter/Setter
	const ColorRGBA& get_color() const { return color; };
	bool is_ended() const { return isEnded; };

	const Matrix4x4 create_uv_matrix() const;

protected: // Member variable
	bool isEnded = false;

	WorldTimer timer;
	r32 lifetime;

	Vector3 velocity;
	Vector3 acceleration;

	ColorRGBA color;
	ColorRGBA startColor;
	ColorRGBA endColor;

	Vector3 startSize;
	Vector3 endSize;

	RotationType rotationType;

	std::variant<Constant, std::monostate, Random, LookAtAngle> rotationData;

	Transform2D uvTransform;

public:
	inline static WorldInstance* lookAtDefault;
};

}; // szg
