#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <imgui.h>
#include <imgui_stdlib.h>

#include <bitset>
#include <string>

class ColorRGB;
class ColorRGBA;

#include <Library/Math/Quaternion.h>
#include <Library/Math/Transform2D.h>
#include <Library/Math/Transform3D.h>
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

template<>
struct show_object<Transform2D> {
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<Transform2D>)

public:
	inline std::bitset<2> show_gui(Transform2D& value) const {
		const ValueEditor::show_object<Vector2> scaleObj{ "Scale" };
		const ValueEditor::show_object<r32> rotateObj{ "Rotate" };
		const ValueEditor::show_object<Vector2> translateObj{ "Translate" };
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		std::bitset<2> result = 0;
		if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanAllColumns)) {
			{
				// ---------- Scale ----------
				Vector2& scale = value.get_scale();
				// リセットボタン
				if (ImGui::Button("\ue5d5##Scale")) {
					scale = CVector2::BASIS;
				}
				if (ImGui::IsItemDeactivated()) {
					result |= 0b10;
				}
				else if (ImGui::IsItemActivated()) {
					result |= 0b01;
				}
				else if (ImGui::IsItemActive()) {
					result |= 0b11;
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(150);
				result |= scaleObj.show_gui(scale);
			}

			{
				// ---------- Rotate ----------
				r32& rotate = value.get_rotate();
				// リセットボタン
				if (ImGui::Button("\ue5d5##Rotate")) {
					rotate = 0;
				}
				if (ImGui::IsItemDeactivated()) {
					result |= 0b10;
				}
				else if (ImGui::IsItemActivated()) {
					result |= 0b01;
				}
				else if (ImGui::IsItemActive()) {
					result |= 0b11;
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(150);
				result |= rotateObj.show_gui(rotate);
			}

			{
				// ---------- Translate ----------
				Vector2& translate = value.get_translate();
				// リセットボタン
				if (ImGui::Button("\ue5d5##Translate")) {
					translate = CVector2::ZERO;
				}
				if (ImGui::IsItemDeactivated()) {
					result |= 0b10;
				}
				else if (ImGui::IsItemActivated()) {
					result |= 0b01;
				}
				else if (ImGui::IsItemActive()) {
					result |= 0b11;
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(150);
				result |= translateObj.show_gui(translate);
			}

			ImGui::TreePop();
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
struct show_object<Transform3D> {
public:
	inline show_object(const std::string& name_) :
		name(name_) {
	};
	~show_object() = default;

	SZG_CLASS_DEFAULT(show_object<Transform3D>)

public:
	inline std::bitset<2> show_gui(Transform3D& value) const {
		const ValueEditor::show_object<Vector3> scaleObj{ "Scale" };
		const ValueEditor::show_object<Quaternion> rotateObj{ "Rotate" };
		const ValueEditor::show_object<Vector3> translateObj{ "Translate" };
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		std::bitset<2> result = 0;
		if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanAllColumns)) {
			{
				// ---------- Scale ----------
				Vector3& scale = value.get_scale();
				// リセットボタン
				if (ImGui::Button("\ue5d5##Scale")) {
					scale = CVector3::BASIS;
				}
				if (ImGui::IsItemDeactivated()) {
					result |= 0b10;
				}
				else if (ImGui::IsItemActivated()) {
					result |= 0b01;
				}
				else if (ImGui::IsItemActive()) {
					result |= 0b11;
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(150);
				result |= scaleObj.show_gui(scale);
			}

			{
				// ---------- Rotate ----------
				Quaternion& rotate = value.get_quaternion();
				// リセットボタン
				if (ImGui::Button("\ue5d5##Rotate")) {
					rotate = CQuaternion::IDENTITY;
				}
				if (ImGui::IsItemDeactivated()) {
					result |= 0b10;
				}
				else if (ImGui::IsItemActivated()) {
					result |= 0b01;
				}
				else if (ImGui::IsItemActive()) {
					result |= 0b11;
				}

				ImGui::SameLine();
				ImGui::Indent(29.f);
				result |= rotateObj.show_gui(rotate);
				ImGui::Unindent(29.f);
			}

			{
				// ---------- Translate ----------
				Vector3& translate = value.get_translate();
				// リセットボタン
				if (ImGui::Button("\ue5d5##Translate")) {
					translate = CVector3::ZERO;
				}
				if (ImGui::IsItemDeactivated()) {
					result |= 0b10;
				}
				else if (ImGui::IsItemActivated()) {
					result |= 0b01;
				}
				else if (ImGui::IsItemActive()) {
					result |= 0b11;
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(150);
				result |= translateObj.show_gui(translate);
			}

			ImGui::TreePop();
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
