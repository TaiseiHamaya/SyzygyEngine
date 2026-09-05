#pragma once

#include <Library/Utility/Template/unorm.h>

#include <json.hpp>

namespace nlohmann {

template<>
struct adl_serializer<unorm> {
	static void to_json(nlohmann::json& j, const unorm& u) {
		j = u.native();
	}

	static void from_json(const nlohmann::json& j, unorm& u) {
		u = unorm::from_native(j.get<unorm::value_type>());
	}
};

}
