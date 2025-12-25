#ifdef DEBUG_FEATURES_ENABLE

#include "RemoteWorldInstance.h"

using namespace szg;

#define TRANSFORM3D_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

RemoteWorldInstance::RemoteWorldInstance() = default;
RemoteWorldInstance::~RemoteWorldInstance() = default;

void RemoteWorldInstance::draw_inspector() {
	ImGui::Text("Type : WorldInstance");

	hierarchyName.show_gui();

	isUseRuntime.show_gui();

	ImGui::Separator();

	transform.show_gui();
}

nlohmann::json RemoteWorldInstance::serialize() const {
	nlohmann::json result;

	result.update(transform);
	result.update(isUseRuntime);
	result.update(hierarchyName);
	result["Type"] = instance_type();
	result["Children"] = nlohmann::json::array();
	for (const auto& child : children) {
		result["Children"].emplace_back(child->serialize());
	}

	return result;
}

#endif // DEBUG_FEATURES_ENABLE
