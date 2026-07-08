#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <bitset>
#include <concepts>
#include <string>

#include <json.hpp>

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"
#include "Engine/Debug/ImGui/ImGuiJsonEditor/ValueEditorObject.h"

namespace szg {

template<typename T, typename ShowObject = ValueEditor::show_object<T>>
class EditorValueField {
public:
	EditorValueField(const std::string& name_, T init = T{}) :
		showObject(name_), value(init) {
	}
	~EditorValueField() = default;

	SZG_CLASS_DEFAULT(EditorValueField<T, ShowObject>)

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

	EditorValueField<T, ShowObject>& operator=(const T& rhs) {
		set(rhs);
		return *this;
	}

	operator const T& () const noexcept {
		return value;
	}

private:
	T value;
	ShowObject showObject;
};

} // namespace szg

namespace nlohmann {

template<typename T, typename U>
	requires std::copyable<T>
struct adl_serializer<szg::EditorValueField<T, U>> {
	static inline void to_json(nlohmann::json& j, const szg::EditorValueField<T, U>& p) {
		j[p.label()] = p.value_imm();
	}

	static inline void from_json(const nlohmann::json& j, szg::EditorValueField<T, U>& p) {
		if (j.contains(p.label())) {
			j[p.label()].get_to(p.value_mut());
		}
	}
};

}

#endif // DEBUG_FEATURES_ENABLE
