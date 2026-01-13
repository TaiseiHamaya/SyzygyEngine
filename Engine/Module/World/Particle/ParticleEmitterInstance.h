#pragma once

#include "../WorldInstance/WorldInstance.h"

#include <list>
#include <memory>
#include <variant>

#include "./DrawSystem/BaseParticleDrawSystem.h"
#include "./Particle/Particle.h"
#include "Engine/Assets/Json/JsonAsset.h"
#include "Engine/Runtime/Clock/WorldTimer.h"

#define VECTOR3_SERIALIZER
#define VECTOR2_SERIALIZER
#define COLOR_RGBA_SERIALIZER
#define QUATERNION_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

namespace szg {

class TextureAsset;

class ParticleEmitterInstance : public WorldInstance {
public:
	template<typename T>
	struct Randomize {
		T min;
		T max;
	};

	struct ParticleInit {
		Randomize<r32> lifetime;
		Randomize<r32> speed;
		struct Direction {
			enum class Mode {
				Constant,
				EmissionShape,
				AngleRange,
			} mode = Mode::Constant;
			struct Constant {
				Vector3 direction;
			};
			struct AngleRange {
				Vector3 baseDirection;
				r32 angle;
			};
			std::variant<Constant, std::monostate, AngleRange> data;
		} direction;
		Randomize<Vector3> acceleration;
		Randomize<Vector3> size;
		struct Rotation {
			Particle::RotationType mode;
			struct Constant {
				Vector3 radian;
				Quaternion rotation;
			};
			struct LookAtAngle {
				Randomize<r32> angleParSec;
				bool isRandomDirection;
			};
			struct Random {
				Randomize<r32> angularVelocity;
			};
			std::variant<Constant, std::monostate, Random, LookAtAngle> data;
		} rotation;
		Randomize<ColorRGBA> color;

		u32 debug_gui(string_literal tag);
	};

	struct ParticleFinal {
		Randomize<Vector3> size;
		Randomize<ColorRGBA> color;

		u32 debug_gui(string_literal tag);
	};

	struct Emission {
		r32 delay;
		u32 count;
		u32 cycles;
		r32 interval;

		struct Shape {
			enum class ShapeType {
				Point,
				Sphere,
				Cone,
				Box,
				//Circle,
			} shapeType{ ShapeType::Point };
			struct Sphere {
				r32 radius;
			};
			struct Cone {
				r32 radius;
				Vector3 direction;
				r32 angle;
			};
			struct Box {
				Vector3 size;
				Vector3 offset;
			};
			std::variant<std::monostate, Sphere, Cone, Box> data;
		} shape;

		u32 debug_gui(string_literal tag);
	};

public: // Constructor/Destructor
	ParticleEmitterInstance(std::filesystem::path jsonFile, u32 MaxParticle);
	virtual ~ParticleEmitterInstance();

public: // Member function
	virtual void update();
	virtual void transfer();
	void draw() const;

	virtual void on_emit(Particle* const) {};
	virtual void restart();

	void emit();
	void end_force();

protected:
	void emit_once();

protected:
	void on_mark_destroy() override;

public:
	bool is_end_all() const;
	bool is_end() const;

private:
	void create_draw_system();

protected: // Member variable
	WorldTimer timer;
	WorldTimer cycleTimer;
	u32 emittedCycle{ 0 };
	bool isLoop;
	bool isParentEmitter;
	r32 duration;

	ParticleDrawType drawType;
	std::variant<std::string, std::shared_ptr<const TextureAsset>> useResourceName;
	struct Rect {
		Vector2 rect;
		Vector2 pivot;
	};
	std::variant<std::monostate, Rect> drawSystemData;

	ParticleInit particleInit;
	ParticleFinal particleFinal;
	Emission emission;

	JsonAsset jsonResource;

private:
	u32 numMaxParticle{ 0 };
	std::list<Reference<Particle>> particles;
	std::unique_ptr<BaseParticleDrawSystem> drawSystem;
};

}; // szg

namespace nlohmann {

template<>
struct adl_serializer<szg::ParticleEmitterInstance::ParticleInit> {
	static void to_json(json& j, const szg::ParticleEmitterInstance::ParticleInit& rhs);

	static void from_json(const json& j, szg::ParticleEmitterInstance::ParticleInit& rhs);
};

template<>
struct adl_serializer<szg::ParticleEmitterInstance::ParticleFinal> {
	static void to_json(json& j, const szg::ParticleEmitterInstance::ParticleFinal& rhs);

	static void from_json(const json& j, szg::ParticleEmitterInstance::ParticleFinal& rhs);
};

template<>
struct adl_serializer<szg::ParticleEmitterInstance::Emission> {
	static void to_json(json& j, const szg::ParticleEmitterInstance::Emission& rhs);

	static void from_json(const json& j, szg::ParticleEmitterInstance::Emission& rhs);
};

inline void adl_serializer<szg::ParticleEmitterInstance::ParticleInit>::to_json(json& j, const szg::ParticleEmitterInstance::ParticleInit& rhs) {
	j["Lifetime"] = nlohmann::json::object();
	j["Lifetime"]["Min"] = rhs.lifetime.min;
	j["Lifetime"]["Max"] = rhs.lifetime.max;
	j["Speed"] = nlohmann::json::object();
	j["Speed"]["Min"] = rhs.speed.min;
	j["Speed"]["Max"] = rhs.speed.max;
	j["Direction"] = nlohmann::json::object();
	j["Direction"]["Mode"] = rhs.direction.mode;
	j["Direction"]["Data"] = nlohmann::json::object();
	switch (rhs.direction.mode) {
	case szg::ParticleEmitterInstance::ParticleInit::Direction::Mode::Constant:
	{
		const auto& data = std::get<szg::ParticleEmitterInstance::ParticleInit::Direction::Constant>(rhs.direction.data);
		j["Direction"]["Data"]["Direction"] = data.direction;
		break;
	}
	case szg::ParticleEmitterInstance::ParticleInit::Direction::Mode::EmissionShape:
		// nothing
		break;
	case szg::ParticleEmitterInstance::ParticleInit::Direction::Mode::AngleRange:
	{
		const auto& data = std::get<szg::ParticleEmitterInstance::ParticleInit::Direction::AngleRange>(rhs.direction.data);
		j["Direction"]["Data"]["Direction"] = data.baseDirection;
		j["Direction"]["Data"]["Angle"] = data.angle;
		break;
	}
	default:
		break;
	}
	j["Acceleration"] = nlohmann::json::object();
	j["Acceleration"]["Min"] = rhs.acceleration.min;
	j["Acceleration"]["Max"] = rhs.acceleration.max;
	j["Size"] = nlohmann::json::object();
	j["Size"]["Min"] = rhs.size.min;
	j["Size"]["Max"] = rhs.size.max;
	j["Rotation"] = nlohmann::json::object();
	j["Rotation"]["Mode"] = rhs.rotation.mode;
	switch (rhs.rotation.mode) {
	case szg::Particle::RotationType::Constant:
	{
		auto& data = std::get<szg::ParticleEmitterInstance::ParticleInit::Rotation::Constant>(rhs.rotation.data);
		j["Rotation"]["Data"] = data.rotation;
		break;
	}
	case szg::Particle::RotationType::Velocity:
	case szg::Particle::RotationType::LookAt:
		break;
	case szg::Particle::RotationType::LookAtAngle:
	{
		auto& data = std::get<szg::ParticleEmitterInstance::ParticleInit::Rotation::LookAtAngle>(rhs.rotation.data);
		j["Rotation"]["Data"] = nlohmann::json::object();
		j["Rotation"]["Data"]["AngleParSec"]["Min"] = data.angleParSec.min;
		j["Rotation"]["Data"]["AngleParSec"]["Max"] = data.angleParSec.max;
		j["Rotation"]["Data"]["IsRandomDirection"] = data.isRandomDirection;
		break;
	}
	case szg::Particle::RotationType::Random:
	{
		auto& data = std::get<szg::ParticleEmitterInstance::ParticleInit::Rotation::Random>(rhs.rotation.data);
		j["Rotation"]["Data"] = nlohmann::json::object();
		j["Rotation"]["Data"]["Min"] = data.angularVelocity.min;
		j["Rotation"]["Data"]["Max"] = data.angularVelocity.max;
		break;
	}
	default:
		break;
	}

	j["Color"] = nlohmann::json::object();
	j["Color"]["Min"] = rhs.color.min;
	j["Color"]["Max"] = rhs.color.max;
}

inline void adl_serializer<szg::ParticleEmitterInstance::ParticleInit>::from_json(const json& j, szg::ParticleEmitterInstance::ParticleInit& rhs) {
	if (j.contains("Lifetime")) {
		if (j["Lifetime"].contains("Min")) {
			rhs.lifetime.min = j["Lifetime"]["Min"];
		}
		if (j["Lifetime"].contains("Max")) {
			rhs.lifetime.max = j["Lifetime"]["Max"];
		}
	}
	if (j.contains("Speed")) {
		if (j["Speed"].contains("Min")) {
			rhs.speed.min = j["Speed"]["Min"];
		}
		if (j["Speed"].contains("Max")) {
			rhs.speed.max = j["Speed"]["Max"];
		}
	}
	if (j.contains("Direction")) {
		if (j["Direction"].contains("Mode")) {
			rhs.direction.mode = j["Direction"]["Mode"];
		}
		if (j["Direction"].contains("Data")) {
			switch (rhs.direction.mode) {
			case szg::ParticleEmitterInstance::ParticleInit::Direction::Mode::Constant:
				rhs.direction.data = szg::ParticleEmitterInstance::ParticleInit::Direction::Constant{ j["Direction"]["Data"]["Direction"] };
				break;
			case szg::ParticleEmitterInstance::ParticleInit::Direction::Mode::EmissionShape:
				rhs.direction.data = std::monostate();
				// nothing
				break;
			case szg::ParticleEmitterInstance::ParticleInit::Direction::Mode::AngleRange:
				rhs.direction.data = szg::ParticleEmitterInstance::ParticleInit::Direction::AngleRange{
					j["Direction"]["Data"]["Direction"],
					j["Direction"]["Data"]["Angle"]
				};
			default:
				break;
			}
		}
	}
	if (j.contains("Acceleration")) {
		if (j["Acceleration"].contains("Min")) {
			rhs.acceleration.min = j["Acceleration"]["Min"];
		}
		if (j["Acceleration"].contains("Max")) {
			rhs.acceleration.max = j["Acceleration"]["Max"];
		}
	}
	if (j.contains("Size")) {
		if (j["Size"].contains("Min")) {
			rhs.size.min = j["Size"]["Min"];
		}
		if (j["Size"].contains("Max")) {
			rhs.size.max = j["Size"]["Max"];
		}
	}
	if (j.contains("Rotation")) {
		if (j["Rotation"].contains("Mode")) {
			rhs.rotation.mode = j["Rotation"]["Mode"];
			switch (rhs.rotation.mode) {
			case szg::Particle::RotationType::Constant:
			{
				//auto& data = std::get<ParticleEmitterInstance::ParticleInit::Rotation::Constant>(rhs.rotation.data);
				rhs.rotation.data = szg::ParticleEmitterInstance::ParticleInit::Rotation::Constant{ j["Rotation"]["Data"] };
				break;
			}
			case szg::Particle::RotationType::Velocity:
			case szg::Particle::RotationType::LookAt:
				rhs.rotation.data = std::monostate();
				break;
			case szg::Particle::RotationType::LookAtAngle:
				rhs.rotation.data = szg::ParticleEmitterInstance::ParticleInit::Rotation::LookAtAngle{
					szg::ParticleEmitterInstance::Randomize<r32>{
					j["Rotation"]["Data"]["AngleParSec"]["Min"],
					j["Rotation"]["Data"]["AngleParSec"]["Max"]
				},
				j["Rotation"]["Data"]["IsRandomDirection"]
				};
				break;
			case szg::Particle::RotationType::Random:
			{
				//auto& data = std::get<ParticleEmitterInstance::ParticleInit::Rotation::Random>(rhs.rotation.data);
				rhs.rotation.data = szg::ParticleEmitterInstance::ParticleInit::Rotation::Random{
					j["Rotation"]["Data"]["Min"],
					j["Rotation"]["Data"]["Max"]
				};
				break;
			}
			default:
				break;
			}
		}
	}
	if (j.contains("Color")) {
		if (j["Color"].contains("Min")) {
			rhs.color.min = j["Color"]["Min"];
		}
		if (j["Color"].contains("Max")) {
			rhs.color.max = j["Color"]["Max"];
		}
	}
}

inline void adl_serializer<szg::ParticleEmitterInstance::ParticleFinal>::to_json(json& j, const szg::ParticleEmitterInstance::ParticleFinal& rhs) {
	j["Color"] = nlohmann::json::object();
	j["Color"]["Min"] = rhs.color.min;
	j["Color"]["Max"] = rhs.color.max;
	j["Size"] = nlohmann::json::object();
	j["Size"]["Min"] = rhs.size.min;
	j["Size"]["Max"] = rhs.size.max;
}

inline void adl_serializer<szg::ParticleEmitterInstance::ParticleFinal>::from_json(const json& j, szg::ParticleEmitterInstance::ParticleFinal& rhs) {
	if (j.contains("Size")) {
		if (j["Size"].contains("Min")) {
			rhs.size.min = j["Size"]["Min"];
		}
		if (j["Size"].contains("Max")) {
			rhs.size.max = j["Size"]["Max"];
		}
	}
	if (j.contains("Color")) {
		if (j["Color"].contains("Min")) {
			rhs.color.min = j["Color"]["Min"];
		}
		if (j["Color"].contains("Max")) {
			rhs.color.max = j["Color"]["Max"];
		}
	}
}

inline void adl_serializer<szg::ParticleEmitterInstance::Emission>::to_json(json& j, const szg::ParticleEmitterInstance::Emission& rhs) {
	j["Delay"] = rhs.delay;
	j["Count"] = rhs.count;
	j["Cycles"] = rhs.cycles;
	j["Interval"] = rhs.interval;
	j["Shape"] = nlohmann::json::object();
	j["Shape"]["Type"] = rhs.shape.shapeType;
	j["Shape"]["Data"] = nlohmann::json::object();
	switch (rhs.shape.shapeType) {
	case szg::ParticleEmitterInstance::Emission::Shape::ShapeType::Point:
		break;
	case szg::ParticleEmitterInstance::Emission::Shape::ShapeType::Sphere:
	{
		const auto& data = std::get<szg::ParticleEmitterInstance::Emission::Shape::Sphere>(rhs.shape.data);
		j["Shape"]["Data"]["Radius"] = data.radius;
		break;
	}
	case szg::ParticleEmitterInstance::Emission::Shape::ShapeType::Cone:
	{
		const auto& data = std::get<szg::ParticleEmitterInstance::Emission::Shape::Cone>(rhs.shape.data);
		j["Shape"]["Data"]["Radius"] = data.radius;
		j["Shape"]["Data"]["Direction"] = data.direction;
		j["Shape"]["Data"]["Angle"] = data.angle;
		break;
	}
	case szg::ParticleEmitterInstance::Emission::Shape::ShapeType::Box:
	{
		const auto& data = std::get<szg::ParticleEmitterInstance::Emission::Shape::Box>(rhs.shape.data);
		j["Shape"]["Data"]["Size"] = data.size;
		j["Shape"]["Data"]["Offset"] = data.offset;
		break;
	}
	default:
		break;
	}
}

inline void adl_serializer<szg::ParticleEmitterInstance::Emission>::from_json(const json& j, szg::ParticleEmitterInstance::Emission& rhs) {
	if (j.contains("Delay")) {
		rhs.delay = j["Delay"];
	}
	if (j.contains("Count")) {
		rhs.count = j["Count"];
	}
	if (j.contains("Cycles")) {
		rhs.cycles = j["Cycles"];
	}
	if (j.contains("Interval")) {
		rhs.interval = j["Interval"];
	}
	if (j.contains("Shape")) {
		if (j["Shape"].contains("Type")) {
			rhs.shape.shapeType = j["Shape"]["Type"];
		}
		if (j["Shape"].contains("Data")) {
			auto& json = j["Shape"]["Data"];
			switch (rhs.shape.shapeType) {
			case szg::ParticleEmitterInstance::Emission::Shape::ShapeType::Point:
				rhs.shape.data = std::monostate();
				break;
			case szg::ParticleEmitterInstance::Emission::Shape::ShapeType::Sphere:
				rhs.shape.data = szg::ParticleEmitterInstance::Emission::Shape::Sphere{
					json["Radius"],
				};
				break;
			case szg::ParticleEmitterInstance::Emission::Shape::ShapeType::Cone:
				rhs.shape.data = szg::ParticleEmitterInstance::Emission::Shape::Cone{
					json["Radius"],
					json["Direction"],
					json["Angle"]
				};
				break;
			case szg::ParticleEmitterInstance::Emission::Shape::ShapeType::Box:
				rhs.shape.data = szg::ParticleEmitterInstance::Emission::Shape::Box{
					json["Size"],
					json["Offset"]
				};
				break;
			default:
				break;
			}
		}
	}
}

};
