#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <bitset>
#include <string>

#include <json.hpp>

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Assets/AssetTypeEnum.h"

namespace szg {

class EditorAssetField {
public:
	EditorAssetField(const std::string& label_, AssetType type, std::string init = "");
	~EditorAssetField() = default;

	SZG_CLASS_DEFAULT(EditorAssetField)

public:
	std::bitset<2> show_gui();

public:
	void set_weak(const std::string& value_);
	std::string& value_mut() { return value; };
	const std::string& value_imm() const { return value; };
	std::string copy() const { return value; }
	std::string_view label_name() const { return label; }

	operator const std::string& () const noexcept;

	void on_activated();
	void on_deactivated();

private:
	std::string label;
	AssetType assetType;

	std::string value{ "Unknown" };
};

} // szg

namespace nlohmann {

template<>
struct adl_serializer<szg::EditorAssetField> {
	static inline void to_json(nlohmann::json& j, const szg::EditorAssetField& p) {
		j[p.label_name()] = p.value_imm();
	}

	static inline void from_json(const nlohmann::json& j, szg::EditorAssetField& p) {
		if (j.contains(p.label_name())) {
			j[p.label_name()].get_to(p.value_mut());
		}
	}
};

}

#endif // DEBUG_FEATURES_ENABLE
