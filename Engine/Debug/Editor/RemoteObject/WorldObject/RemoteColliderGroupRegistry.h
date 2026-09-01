#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <optional>
#include <string>
#include <vector>

#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class RemoteColliderGroupRegistry final {
public:
	RemoteColliderGroupRegistry() = default;
	~RemoteColliderGroupRegistry() = default;

	SZG_CLASS_MOVE_ONLY(RemoteColliderGroupRegistry)

private:
	struct ColliderGroupEntry {
		std::string name;
		bool isDeleted{ false };
	};

public:
	void load(const std::string& sceneName);

	void show_gui();

	std::optional<i32> combo_gui(const i32 current, const std::string& label = "") const;

private:
	bool isOpen{ true };

	std::optional<std::string> renameGroupName;
	std::optional<i32> selecteIdx;

	std::vector<ColliderGroupEntry> colliderGroups;

	std::vector<i32> groupEditorGuiOrder;
};

}; // namespace szg

#endif // DEBUG_FEATURES_ENABLE
