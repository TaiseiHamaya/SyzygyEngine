#ifdef DEBUG_FEATURES_ENABLE
#include "ImGuiLoadManager.h"

#include <format>
#include <imgui.h>

#include "Engine/Assets/Animation/NodeAnimation/NodeAnimationLibrary.h"
#include "Engine/Assets/Animation/Skeleton/SkeletonLibrary.h"
#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"
#include "Engine/Assets/Texture/TextureLibrary.h"

using namespace szg;

ImGuiLoadManager::ImGuiLoadManager() {
	meshCurrentPath = "./SyzygyEngine/Assets";
	textureCurrentPath = "./SyzygyEngine/Assets";
	get_file_list(meshFileList, meshCurrentPath, { ".obj", ".gltf" });
	get_file_list(textureFileList, textureCurrentPath, { ".png" });
};

ImGuiLoadManager::~ImGuiLoadManager() noexcept = default;

ImGuiLoadManager& ImGuiLoadManager::GetInstance() {
	static ImGuiLoadManager instance{};
	return instance;
}

void ImGuiLoadManager::ShowGUI() {
	GetInstance().show_gui();
}

void ImGuiLoadManager::show_gui() {
	ImGuiID dock = ImGui::GetID("LoadManagerDock");
	// メッシュ用
	//ImGui::SetNextWindowSize(ImVec2{ 330,320 }, ImGuiCond_Once);
	//ImGui::SetNextWindowPos(ImVec2{ 20, 355 }, ImGuiCond_Once);
	ImGui::SetNextWindowDockID(dock, ImGuiCond_FirstUseEver);
	ImGui::Begin("PolygonMeshLoad", nullptr);
	if (ImGui::Button("<-")) {
		if (meshCurrentPath != ".") {
			meshCurrentPath = meshCurrentPath.parent_path();
			get_file_list(meshFileList, meshCurrentPath, { ".obj", ".gltf" });
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Reload")) {
		get_file_list(meshFileList, meshCurrentPath, { ".obj", ".gltf" });
	}
	ImGui::SameLine();
	ImGui::Text(std::format("Current-\'{:}\'", meshCurrentPath.string()).c_str());
	for (u32 i = 0; i < meshFileList.size(); ++i) {
		if (ImGui::Button(meshFileList[i].c_str())) {
			std::filesystem::path select = meshCurrentPath / meshFileList[i];
			if (std::filesystem::is_directory(select)) {
				meshCurrentPath = std::move(select);
				get_file_list(meshFileList, meshCurrentPath, { ".obj", ".gltf" });
			}
			else if (std::filesystem::exists(select)) {
				PolygonMeshLibrary::RegisterLoadQue(select);
				NodeAnimationLibrary::RegisterLoadQue(select);
				SkeletonLibrary::RegisterLoadQue(select);
			}
		}
	}
	ImGui::End();

	// テクスチャ用
	//ImGui::SetNextWindowSize(ImVec2{ 330,320 }, ImGuiCond_Once);
	//ImGui::SetNextWindowPos(ImVec2{ 20, 355 }, ImGuiCond_Once);
	ImGui::SetNextWindowDockID(dock, ImGuiCond_FirstUseEver);
	ImGui::Begin("TextureLoad", nullptr);
	if (ImGui::Button("<-")) {
		if (textureCurrentPath != ".") {
			textureCurrentPath = textureCurrentPath.parent_path();
			get_file_list(textureFileList, textureCurrentPath, { ".png" });
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Reload")) {
		get_file_list(textureFileList, textureCurrentPath, { ".png" });
	}
	ImGui::SameLine();
	ImGui::Text(std::format("Current-\'{:}\'", textureCurrentPath.string()).c_str());
	for (u32 i = 0; i < textureFileList.size(); ++i) {
		if (ImGui::Button(textureFileList[i].c_str())) {
			std::filesystem::path select = textureCurrentPath / textureFileList[i];
			if (std::filesystem::is_directory(select)) {
				textureCurrentPath = std::move(select);
				get_file_list(textureFileList, textureCurrentPath, { ".png" });
			}
			else if (std::filesystem::exists(select)) {
				TextureLibrary::RegisterLoadQue(select);
			}
		}
	}
	ImGui::End();
}

void ImGuiLoadManager::get_file_list(std::vector<std::string>& list, const std::filesystem::path& path, std::unordered_set<std::string> extensions) {

	std::filesystem::directory_iterator folderItr{ path };
	std::filesystem::directory_iterator end;

	list.clear();

	std::vector<std::string> folders;
	while (folderItr != end) {
		if (folderItr->is_directory()) {
			folders.emplace_back(folderItr->path().filename().string());
		}
		++folderItr;
	}

	std::filesystem::directory_iterator fileItr{ path };

	std::vector<std::string> files;
	while (fileItr != end) {
		if (extensions.contains(fileItr->path().extension().string())) {
			files.emplace_back(fileItr->path().filename().string());
		}
		++fileItr;
	}

	std::sort(folders.begin(), folders.end());
	std::sort(files.begin(), files.end());

	list = std::move(folders);
	list.insert(list.end(), files.begin(), files.end());
}
#endif // _DEBUG
