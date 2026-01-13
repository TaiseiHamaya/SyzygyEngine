#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <imgui.h>
#include <imgui_stdlib.h>

#include <bitset>
#include <string>

class Vector2;
class Vector3;
class ColorRGB;
class ColorRGBA;

#include <Library/Math/Quaternion.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

namespace ValueEditor {

template<typename T>
struct show_object;

// bool
template<>
struct show_object<bool> {
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<bool>)

public:
	inline std::bitset<2> show_gui(bool& value) const {
		ImGui::Checkbox(name.c_str(), &value);
		std::bitset<2> result = 0;
		if (ImGui::IsItemDeactivated()) {
			result = 0b10;
		}
		else if (ImGui::IsItemActivated()) {
			result = 0b01;
		}
		else if (ImGui::IsItemActive()) {
			result = 0b11;
		}
		return result;
	};

	std::string_view get_name() const {
		return name;
	}

private:
	std::string name;
};

// r32
template<>
struct show_object<r32> {
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<r32>)

public:
	inline std::bitset<2> show_gui(r32& value) const {
		ImGui::DragFloat(name.c_str(), &value, 0.1f);
		std::bitset<2> result = 0;
		if (ImGui::IsItemDeactivated()) {
			result = 0b10;
		}
		else if (ImGui::IsItemActivated()) {
			result = 0b01;
		}
		else if (ImGui::IsItemActive()) {
			result = 0b11;
		}
		return result;
	};

	std::string_view get_name() const {
		return name;
	}

private:
	std::string name;
};

// i32
template<>
struct show_object<i32> {
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<i32>)

public:
	inline std::bitset<2> show_gui(i32& value) const {
		constexpr i32 step = 1;
		constexpr i32 fastStep = 10;
		ImGui::InputScalar(name.c_str(), ImGuiDataType_S32, &value, &step, &fastStep);
		std::bitset<2> result = 0;
		if (ImGui::IsItemDeactivated()) {
			result = 0b10;
		}
		else if (ImGui::IsItemActivated()) {
			result = 0b01;
		}
		else if (ImGui::IsItemActive()) {
			result = 0b11;
		}
		return result;
	};

	std::string_view get_name() const {
		return name;
	}

private:
	std::string name;
};

// u32
template<>
struct show_object<u32> {
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<u32>)

public:
	inline std::bitset<2> show_gui(u32& value) const {
		constexpr i32 step = 1;
		constexpr i32 fastStep = 10;
		ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &value, &step, &fastStep);
		std::bitset<2> result = 0;
		if (ImGui::IsItemDeactivated()) {
			result = 0b10;
		}
		else if (ImGui::IsItemActivated()) {
			result = 0b01;
		}
		else if (ImGui::IsItemActive()) {
			result = 0b11;
		}
		return result;
	};

	std::string_view get_name() const {
		return name;
	}

private:
	std::string name;
};

// Vector2
template<>
struct show_object<Vector2> {
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<Vector2>)

public:
	inline std::bitset<2> show_gui(Vector2& value) const {
		ImGui::DragFloat2(name.c_str(), reinterpret_cast<r32*>(&value), 0.1f);
		std::bitset<2> result = 0;
		if (ImGui::IsItemDeactivated()) {
			result = 0b10;
		}
		else if (ImGui::IsItemActivated()) {
			result = 0b01;
		}
		else if (ImGui::IsItemActive()) {
			result = 0b11;
		}
		return result;
	};

	std::string_view get_name() const {
		return name;
	}

private:
	std::string name;
};

// Vector3
template<>
struct show_object<Vector3> {
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<Vector3>)

public:
	inline std::bitset<2> show_gui(Vector3& value) const {
		ImGui::DragFloat3(name.c_str(), reinterpret_cast<r32*>(&value), 0.1f);
		std::bitset<2> result = 0;
		if (ImGui::IsItemDeactivated()) {
			result = 0b10;
		}
		else if (ImGui::IsItemActivated()) {
			result = 0b01;
		}
		else if (ImGui::IsItemActive()) {
			result = 0b11;
		}
		return result;
	};

	std::string_view get_name() const {
		return name;
	}

private:
	std::string name;
};

// Quaternion
template<>
struct show_object<Quaternion> {
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<Quaternion>)

public:
	inline std::bitset<2> show_gui(Quaternion& value) const {
		std::bitset<2> result1 = 0;
		Vector3 rotationL = CVector3::ZERO;
		ImGui::SetNextItemWidth(150);
		if (ImGui::DragFloat3("RotateLocal", &rotationL.x, 1.0f, -180.0f, 180.0f, "")) {
			value = (value * Quaternion::EulerDegree(rotationL)).normalize();
		}
		if (ImGui::IsItemDeactivated()) {
			result1 = 0b10;
		}
		else if (ImGui::IsItemActivated()) {
			result1 = 0b01;
		}
		else if (ImGui::IsItemActive()) {
			result1 = 0b11;
		}

		std::bitset<2> result2 = 0;
		Vector3 rotationW = CVector3::ZERO;
		ImGui::SetNextItemWidth(150);
		if (ImGui::DragFloat3("RotateWorld", &rotationW.x, 1.0f, -180.0f, 180.0f, "")) {
			value *= Quaternion::EulerDegree(rotationW);
			value = value.normalize();
		}
		if (ImGui::IsItemDeactivated()) {
			result2 = 0b10;
		}
		else if (ImGui::IsItemActivated()) {
			result2 = 0b01;
		}
		else if (ImGui::IsItemActive()) {
			result2 = 0b11;
		}

		return result1 | result2;
	};

	std::string_view get_name() const {
		return name;
	}

private:
	std::string name;
};

template<>
struct show_object<std::string> {
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<std::string>)

public:
	inline std::bitset<2> show_gui(std::string& value) const {
		ImGui::InputText(name.c_str(), &value);
		std::bitset<2> result = 0;
		if (ImGui::IsItemDeactivated()) {
			result = 0b10;
		}
		else if (ImGui::IsItemActivated()) {
			result = 0b01;
		}
		else if (ImGui::IsItemActive()) {
			result = 0b11;
		}
		return result;
	};

	std::string_view get_name() const {
		return name;
	}

private:
	std::string name;
};

template<>
struct show_object<ColorRGB> {
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<ColorRGB>)

public:
	inline std::bitset<2> show_gui(ColorRGB& value) const {
		ImGui::ColorEdit3(name.c_str(), reinterpret_cast<r32*>(&value), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_InputRGB);
		std::bitset<2> result = 0;
		if (ImGui::IsItemDeactivated()) {
			result = 0b10;
		}
		else if (ImGui::IsItemActivated()) {
			result = 0b01;
		}
		else if (ImGui::IsItemActive()) {
			result = 0b11;
		}
		return result;
	};

	std::string_view get_name() const {
		return name;
	}

private:
	std::string name;
};

template<>
struct show_object<ColorRGBA> {
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<ColorRGBA>)

public:
	inline std::bitset<2> show_gui(ColorRGBA& value) const {
		ImGui::ColorEdit4(name.c_str(), reinterpret_cast<r32*>(&value), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_InputRGB);
		std::bitset<2> result = 0;
		if (ImGui::IsItemDeactivated()) {
			result = 0b10;
		}
		else if (ImGui::IsItemActivated()) {
			result = 0b01;
		}
		else if (ImGui::IsItemActive()) {
			result = 0b11;
		}
		return result;
	};

	std::string_view get_name() const {
		return name;
	}

private:
	std::string name;
};

// debug_gui()関数があるクラス
template<typename T>
concept HasDebugGui = requires(T t, string_literal tag) {
	{ t.debug_gui(tag) } -> std::convertible_to<std::bitset<2>>;
};

template<typename T>
	requires HasDebugGui<T>
struct show_object<T> {
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<T>)

public:
	inline std::bitset<2> show_gui(T& value) const {
		return value.debug_gui(name.c_str());
	};

	std::string_view get_name() const {
		return name;
	}

private:
	std::string name;
};

}

}; // szg

#endif // _DEBUG
