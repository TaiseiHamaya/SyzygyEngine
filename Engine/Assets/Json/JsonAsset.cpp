#include "JsonAsset.h"

#include <fstream>

#include "Engine/Application/Logger.h"

#include "../IAssetBuilder.h"

using namespace szg;

JsonAsset::JsonAsset(const std::filesystem::path& file) {
	load(file);
}

void JsonAsset::load(const std::filesystem::path& file) {
	filePath = IAssetBuilder::ResolveFilePath(file, "json");

	if (!std::filesystem::exists(filePath)) {
		szgWarning(L"File-\'{}\' is not found.", filePath.stem().native());
		return;
	}

	std::ifstream ifstream{ filePath };

	if (ifstream.fail()) {
		szgError(L"File-\'{}\' is not found.", filePath.stem().native());
		return;
	}

	json = nlohmann::json::parse(ifstream);
}

void JsonAsset::save() const {
#ifdef DEBUG_FEATURES_ENABLE
	valueEditor.save_all();
#endif // _DEBUG

	auto parentPath = filePath.parent_path();
	if (!parentPath.empty() && !std::filesystem::exists(parentPath)) {
		std::filesystem::create_directories(parentPath);
	}

	std::ofstream ofstream{ filePath, std::ios_base::out };
	ofstream << std::setw(1) << std::setfill('\t') << json;
	ofstream.close();
}

nlohmann::json& JsonAsset::get() {
	return json;
}

const nlohmann::json& JsonAsset::cget() const {
	return json;
}

#ifdef DEBUG_FEATURES_ENABLE

void JsonAsset::editor_gui() {
	valueEditor.show_imgui();
}

void JsonAsset::show_imgui() {
	valueEditor.show_imgui();
	if (ImGui::Button("Save")) {
		save();
	}
}

#endif // _DEBUG
