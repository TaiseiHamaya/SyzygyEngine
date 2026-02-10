#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <string>
#include <bitset>

#include <json.hpp>

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Assets/AssetTypeEnum.h"

namespace szg {

class EditorAssetField {
	friend struct ::nlohmann::adl_serializer<EditorAssetField>;

	using GUIFuncType = std::function<bool(std::string&)>;

public:
	EditorAssetField(const std::string& label_, AssetType type);
	~EditorAssetField() = default;

	SZG_CLASS_DEFAULT(EditorAssetField)

public:
	std::bitset<2> show_gui();

public:
	void set_weak(const std::string& value_);
	void set(const std::string& value_);
	std::string& value_mut() { return value; };
	const std::string& value_imm() const { return value; };
	std::string copy() const { return value; }
	std::string_view label_name() const { return label; }

	EditorAssetField& operator=(const std::string& rhs);

	operator const std::string& () const noexcept;

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
		j[p.label] = p.value;
	}

	static inline void from_json(const nlohmann::json& j, szg::EditorAssetField& p) {
		if (j.contains(p.label)) {
			j[p.label].get_to(p.value);
		}
	}
};

}

#endif // DEBUG_FEATURES_ENABLE
