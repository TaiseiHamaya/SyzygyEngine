#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <concepts>
#include <format>
#include <string>

#include <json.hpp>

#include <Library/Math/Quaternion.h>
#include <Library/Math/Transform2D.h>
#include <Library/Math/Transform3D.h>
#include <Library/Math/Vector3.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Debug/Editor/Command/EditorCommandScope.h"
#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"
#include "Engine/Debug/ImGui/ImGuiJsonEditor/ValueEditorObject.h"

#define TRANSFORM3D_SERIALIZER
#define TRANSFORM2D_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

namespace szg {

template<typename T>
class EditorValueField {
public:
	EditorValueField(const std::string& name_, T init = T{}) :
		showObject(name_), value(init) {
	}
	~EditorValueField() = default;

	SZG_CLASS_DEFAULT(EditorValueField<T>)

public:
	std::bitset<2> show_gui() {
		T temp = value;
		std::bitset<2> result = showObject.show_gui(temp);
		if (result == 0b01) {
			EditorValueChangeCommandHandler::GenCommand<T>(value);
		}
		value = temp;
		if (result == 0b10) {
			EditorValueChangeCommandHandler::End();
		}
		return result;
	}

	friend struct ::nlohmann::adl_serializer<EditorValueField<T>>;

public:
	void set_weak(const T& value_) {
		value = value_;
	}
	void set(const T& value_) {
		EditorValueChangeCommandHandler::GenCommand<T>(value);
		value = value_;
		EditorValueChangeCommandHandler::End();
	}
	T& value_mut() { return value; };
	const T& value_imm() const { return value; };
	T copy() const { return value; }
	std::string_view label() const {
		return showObject.get_name();
	}

	EditorValueField<T>& operator=(const T& rhs) {
		set(rhs);
		return *this;
	}

	operator const T&() const noexcept {
		return value;
	}

private:
	T value;
	ValueEditor::show_object<T> showObject;
};

void Transform3DShowGuiBody(const std::string& gui_label, Transform3D& transform);

template<>
class EditorValueField<Transform3D> {
public:
	EditorValueField(const std::string& name_) :
		gui_label(name_) {
	};
	~EditorValueField() = default;

	SZG_CLASS_DEFAULT(EditorValueField<Transform3D>)

public:
	void show_gui() {
		Transform3DShowGuiBody(
			std::format("{}##{:}", gui_label, (void*)this),
			value
		);
	}

	Transform3D& get() { return value; };
	const Transform3D& cget() const { return value; };
	void set_weak(const Transform3D& value_) {
		value.copy(value_);
	}
	void set(const Transform3D& value_) {
		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());

		EditorValueChangeCommandHandler::GenCommand<Vector3>(value.get_scale());
		value.set_scale(value_.get_scale());
		EditorValueChangeCommandHandler::End();
		EditorValueChangeCommandHandler::GenCommand<Quaternion>(value.get_quaternion());
		value.set_quaternion(value_.get_quaternion());
		EditorValueChangeCommandHandler::End();
		EditorValueChangeCommandHandler::GenCommand<Vector3>(value.get_translate());
		value.set_translate(value_.get_translate());
		EditorValueChangeCommandHandler::End();

		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
	};
	Transform3D copy() const {
		Transform3D copy;
		copy.copy(value);
		return copy;
	}
	std::string_view label() const {
		return gui_label;
	}

	EditorValueField<Transform3D>& operator=(const Transform3D& rhs) {
		set(rhs);
		return *this;
	}

	operator const Transform3D&() const {
		return value;
	}

	friend struct ::nlohmann::adl_serializer<EditorValueField<Transform3D>>;

private:
	std::string gui_label;

	Transform3D value;
};

void Transform2DShowGuiBody(const std::string& gui_label, Transform2D& transform);

template<>
class EditorValueField<Transform2D> {
public:
	EditorValueField(const std::string& name_) :
		gui_label(name_) {
	};
	~EditorValueField() = default;

	SZG_CLASS_DEFAULT(EditorValueField<Transform2D>)

public:
	void show_gui() {
		Transform2DShowGuiBody(
			std::format("{}##{:}", gui_label, (void*)this),
			value
		);
	}

	Transform2D& get() { return value; };
	const Transform2D& cget() const { return value; };
	void set_weak(const Transform2D& value_) {
		value.copy(value_);
	}
	void set(const Transform2D& value_) {
		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());

		EditorValueChangeCommandHandler::GenCommand<Vector2>(value.get_scale());
		value.set_scale(value_.get_scale());
		EditorValueChangeCommandHandler::End();
		EditorValueChangeCommandHandler::GenCommand<r32>(value.get_rotate());
		value.set_rotate(value_.get_rotate());
		EditorValueChangeCommandHandler::End();
		EditorValueChangeCommandHandler::GenCommand<Vector2>(value.get_translate());
		value.set_translate(value_.get_translate());
		EditorValueChangeCommandHandler::End();

		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
	};
	Transform2D copy() const {
		Transform2D copy;
		copy.copy(value);
		return copy;
	}
	std::string_view label() const {
		return gui_label;
	}

	EditorValueField<Transform2D>& operator=(const Transform2D& rhs) {
		set(rhs);
		return *this;
	}

	operator const Transform2D& () const {
		return value;
	}

	friend struct ::nlohmann::adl_serializer<EditorValueField<Transform2D>>;

private:
	std::string gui_label;

	Transform2D value;
};

} // namespace szg

namespace nlohmann {

template<typename T>
	requires std::copyable<T>
struct adl_serializer<szg::EditorValueField<T>> {
	static inline void to_json(nlohmann::json& j, const szg::EditorValueField<T>& p) {
		j[p.label()] = p.value;
	}

	static inline void from_json(const nlohmann::json& j, szg::EditorValueField<T>& p) {
		if (j.contains(p.label())) {
			j[p.label()].get_to(p.value);
		}
	}
};

template<>
struct adl_serializer<szg::EditorValueField<Transform3D>> {
	static inline void to_json(nlohmann::json& j, const szg::EditorValueField<Transform3D>& p) {
		j[p.label()] = p.value;
	}

	static inline void from_json(const nlohmann::json& j, szg::EditorValueField<Transform3D>& p) {
		if (j.contains(p.label())) {
			j[p.label()].get_to(p.value);
		}
	}
};

template<>
struct adl_serializer<szg::EditorValueField<Transform2D>> {
	static inline void to_json(nlohmann::json& j, const szg::EditorValueField<Transform2D>& p) {
		j[p.label()] = p.value;
	}

	static inline void from_json(const nlohmann::json& j, szg::EditorValueField<Transform2D>& p) {
		if (j.contains(p.label())) {
			j[p.label()].get_to(p.value);
		}
	}
};

}

#endif // DEBUG_FEATURES_ENABLE
