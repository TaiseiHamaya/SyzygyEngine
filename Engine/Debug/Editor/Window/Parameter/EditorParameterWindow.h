#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IEditorWindow.h"
#include "Parameter.h"

#include <filesystem>

#include <Library/Utility/SparseSet.h>

namespace szg {

class EditorParameterWindow final : public IEditorWindow {
public:
	EditorParameterWindow() = default;
	~EditorParameterWindow() = default;

	SZG_CLASS_MOVE_ONLY(EditorParameterWindow)

public:
	void draw() override;

private:
	void save() const;

	void load();

private:
	std::optional<std::filesystem::path> filename;
	bool isEditingFilename{ false };

	std::optional<i64> paramSelectedId;
	szg::SparseSet<Parameter> parameters;

	std::optional<std::string> stringEditBuffer;
};

}

#endif // DEBUG_FEATURES_ENABLE
