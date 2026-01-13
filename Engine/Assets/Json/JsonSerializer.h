#include <json.hpp>

#ifdef TRANSFORM3D_SERIALIZER
#define QUATERNION_SERIALIZER
#define VECTOR3_SERIALIZER
#endif // TRANSFORM3D_SERIALIZER

#ifdef QUATERNION_SERIALIZER
#define VECTOR3_SERIALIZER
#endif // QUATERNION_SERIALIZER

#ifdef TRANSFORM2D_SERIALIZER
#define VECTOR2_SERIALIZER
#endif // TRANSFORM3D_SERIALIZER

namespace nlohmann {
// Vector3 <-> Json Serializer
#ifdef VECTOR3_SERIALIZER
#ifndef VECTOR3_SERIALIZER_DEFINED
#define VECTOR3_SERIALIZER_DEFINED
#include <Library/Math/Vector3.h>
template<>
struct adl_serializer<Vector3> {
	static void to_json(json& j, const Vector3& rhs) {
		j["X"] = rhs.x;
		j["Y"] = rhs.y;
		j["Z"] = rhs.z;
	}

	static void from_json(const json& j, Vector3& rhs) {
		if (j.contains("X") && j.at("X").is_number_float()) {
			rhs.x = j["X"];
		}
		if (j.contains("Y") && j.at("Y").is_number_float()) {
			rhs.y = j["Y"];
		}
		if (j.contains("Z") && j.at("Z").is_number_float()) {
			rhs.z = j["Z"];
		}
	}
};
#endif // VECTOR3_SERIALIZER_DEFINED
#endif // VECTOR3_SERIALIZER

// Vector2 <-> Json Serializer
#ifdef VECTOR2_SERIALIZER
#ifndef VECTOR2_SERIALIZER_DEFINED
#define VECTOR2_SERIALIZER_DEFINED
#include <Library/Math/Vector2.h>
template<>
struct adl_serializer<Vector2> {
	static void to_json(json& j, const Vector2& rhs) {
		j["X"] = rhs.x;
		j["Y"] = rhs.y;
	}

	static void from_json(const json& j, Vector2& rhs) {
		if (j.contains("X") && j.at("X").is_number_float()) {
			rhs.x = j["X"];
		}
		if (j.contains("Y") && j.at("Y").is_number_float()) {
			rhs.y = j["Y"];
		}
	}
};
#endif // VECTOR2_SERIALIZER_DEFINED
#endif // VECTOR2_SERIALIZER

// ColorRGB <-> Json Serializer
#ifdef COLOR_RGB_SERIALIZER
#ifndef COLOR_RGB_SERIALIZER_DEFINED
#define COLOR_RGB_SERIALIZER_DEFINED
#include <Library/Math/ColorRGB.h>
template<>
struct adl_serializer<ColorRGB> {
	static void to_json(json& j, const ColorRGB& rhs) {
		j["R"] = rhs.red;
		j["G"] = rhs.green;
		j["B"] = rhs.blue;
	}

	static void from_json(const json& j, ColorRGB& rhs) {
		if (j.contains("R") && j.at("R").is_number_float()) {
			rhs.red = j["R"];
		}
		if (j.contains("G") && j.at("G").is_number_float()) {
			rhs.green = j["G"];
		}
		if (j.contains("B") && j.at("B").is_number_float()) {
			rhs.blue = j["B"];
		}
	}
};
#endif // COLOR_RGB_SERIALIZER_DEFINED
#endif // COLOR_RGB_SERIALIZER

// ColorRGBA <-> Json Serializer
#ifdef COLOR_RGBA_SERIALIZER
#ifndef COLOR_RGBA_SERIALIZER_DEFINED
#define COLOR_RGBA_SERIALIZER_DEFINED
#include <Library/Math/ColorRGBA.h>
template<>
struct adl_serializer<ColorRGBA> {
	static void to_json(json& j, const ColorRGBA& rhs) {
		j["R"] = rhs.red;
		j["G"] = rhs.green;
		j["B"] = rhs.blue;
		j["A"] = rhs.alpha;
	}

	static void from_json(const json& j, ColorRGBA& rhs) {
		if (j.contains("R") && j.at("R").is_number_float()) {
			rhs.red = j["R"];
		}
		if (j.contains("G") && j.at("G").is_number_float()) {
			rhs.green = j["G"];
		}
		if (j.contains("B") && j.at("B").is_number_float()) {
			rhs.blue = j["B"];
		}
		if (j.contains("A") && j.at("A").is_number_float()) {
			rhs.alpha = j["A"];
		}
	}
};
#endif // COLOR_RGBA_SERIALIZER_DEFINED
#endif // COLOR_RGBA_SERIALIZER

// Quaternion <-> Json Serializer
#ifdef QUATERNION_SERIALIZER
#ifndef QUATERNION_SERIALIZER_DEFINED
#define QUATERNION_SERIALIZER_DEFINED
#include <Library/Math/Quaternion.h>
template<>
struct adl_serializer<Quaternion> {
	static void to_json(json& j, const Quaternion& rhs) {
		j["XYZ"] = rhs.vector();
		j["W"] = rhs.real();
	}

	static void from_json(const json& j, Quaternion& rhs) {
		Vector3 xyz{};
		r32 w = 0;
		if (j.contains("XYZ") && j.at("XYZ").is_object()) {
			xyz = j["XYZ"];
		}
		if (j.contains("W") && j.at("W").is_number_float()) {
			w = j["W"];
		}
		rhs = Quaternion{ xyz, w };
	}
};
#endif // QUATERNION_SERIALIZER_DEFINED
#endif // QUATERNION_SERIALIZER

#ifdef TRANSFORM3D_SERIALIZER
#ifndef TRANSFORM3D_SERIALIZER_DEFINED
#define TRANSFORM3D_SERIALIZER_DEFINED
#include <Library/Math/Transform3D.h>
template<>
struct adl_serializer<Transform3D> {
	static void to_json(json& j, const Transform3D& rhs) {
		j["Scale"] = rhs.get_scale();
		j["Rotation"] = rhs.get_quaternion();
		j["Translate"] = rhs.get_translate();
	}

	static void from_json(const json& j, Transform3D& rhs) {
		if (j.contains("Scale")) {
			rhs.set_scale(j["Scale"].get<Vector3>());
		}
		if (j.contains("Rotation")) {
			rhs.set_quaternion(j["Rotation"].get<Quaternion>());
		}
		if (j.contains("Translate")) {
			rhs.set_translate(j["Translate"].get<Vector3>());
		}
	}
};
#endif // !TRANSFORM3D_SERIALIZER_DEFINED
#endif // TRANSFORM3D_SERIALIZER

#ifdef TRANSFORM2D_SERIALIZER
#ifndef TRANSFORM2D_SERIALIZER_DEFINED
#define TRANSFORM2D_SERIALIZER_DEFINED
#include <Library/Math/Transform2D.h>
template<>
struct adl_serializer<Transform2D> {
	static void to_json(json& j, const Transform2D& rhs) {
		j["Scale"] = rhs.get_scale();
		j["Rotation"] = rhs.get_rotate();
		j["Translate"] = rhs.get_translate();
	}

	static void from_json(const json& j, Transform2D& rhs) {
		if (j.contains("Scale")) {
			rhs.set_scale(j["Scale"].get<Vector2>());
		}
		if (j.contains("Rotation")) {
			rhs.set_rotate(j["Rotation"].get<r32>());
		}
		if (j.contains("Translate")) {
			rhs.set_translate(j["Translate"].get<Vector2>());
		}
	}
};
#endif // TRANSFORM2D_SERIALIZER_DEFINED
#endif // TRANSFORM3D_SERIALIZER

};
