#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <concepts>
#include <string>

#include <json.hpp>

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"
#include "Engine/Debug/ImGui/ImGuiJsonEditor/ValueEditorObject.h"

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

}

#endif // DEBUG_FEATURES_ENABLE
