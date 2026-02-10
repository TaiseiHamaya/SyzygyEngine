#include "EditorAssetField.h"

#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"
#include "Engine/Debug/Editor/Core/EditorAssetContentsCollector.h"

szg::EditorAssetField::EditorAssetField(const std::string& label_, AssetType type) :
	label(label_),
	assetType(type) {
}

std::bitset<2> szg::EditorAssetField::show_gui() {
	auto result = EditorAssetContentsCollector::ComboGUI(value, assetType, label);

	if (result.has_value()) {
		EditorValueChangeCommandHandler::GenCommandInstant<std::string>(value, result.value());
		return 0b10;
	}
	else {
		return 0b00;
	}
}

void szg::EditorAssetField::set_weak(const std::string& value_) {
	value = value_;
}

void szg::EditorAssetField::set(const std::string& value_) {
	EditorValueChangeCommandHandler::GenCommand<std::string>(value);
	value = value_;
	EditorValueChangeCommandHandler::End();
}

szg::EditorAssetField& szg::EditorAssetField::operator=(const std::string& rhs) {
	set(rhs);
	return *this;
}

szg::EditorAssetField::operator const std::string& () const noexcept {
	return value;
}
