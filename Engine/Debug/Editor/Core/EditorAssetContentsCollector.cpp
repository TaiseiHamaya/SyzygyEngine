#include "EditorAssetContentsCollector.h"

#include <ranges>

#include <imgui.h>

#include "Engine/Assets/AssetRootPath.h"

#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"

void szg::EditorAssetContentsCollector::Finalize() {
	auto& instance = GetInstance();
	if (instance.thread.joinable()) {
		instance.thread.join();
	}
}

void szg::EditorAssetContentsCollector::Update() {
	auto& instance = GetInstance();
	// 全ファイルの捜査
	instance.timer.back();
	if (instance.timer <= 0) {
		if (instance.thread.joinable()) {
			instance.thread.join();
		}
		instance.thread = std::thread([&instance]() {
			instance.collect_assets();
		});
		instance.timer.set(1.0f); // 1秒ごとに更新
	}

	// 削除された要素を除外
	std::lock_guard<std::mutex> lock(instance.mutex);
	for (auto& typeMap : instance.assetMaps) {
		std::erase_if(typeMap, [](const auto& pair) {
			return !std::filesystem::exists(pair.second.path);
		});
}
}

std::optional<std::string> szg::EditorAssetContentsCollector::ComboGUI(const std::string& current, AssetType type, const std::string& label) {
	std::optional<std::string> result;
	auto& instance = GetInstance();

	// ラベル名の決定
	std::string labelName = label.empty() ? ASSET_TYPE_NAME[static_cast<i32>(type)] : label;

	// コンボボックスの表示
	if (ImGui::BeginCombo(labelName.c_str(), current.c_str())) {
		std::lock_guard<std::mutex> lock(instance.mutex);
		for (const auto& asset : instance.assetMaps[static_cast<i32>(type)] | std::views::values) {
			bool isSelected = (current == asset.name); // 選択済みかどうか
			if (ImGui::Selectable(asset.name.c_str(), isSelected)) {
				if (!isSelected) { // すでに選択されているものを選択しても変更とみなさない
					result = asset.name;
				}
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();

	}
	return result;
}

szg::AssetType szg::EditorAssetContentsCollector::GetAssetTypeByExtension(const std::string& extension) {
			AssetType assetType = AssetType::Unknown;
			std::string extension = directory.path().extension().string();
			// 拡張子からAssetTypeを決定
			if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp" || extension == ".dds") {
				assetType = AssetType::Texture;
			}
			else if (extension == ".obj" || extension == ".fbx" || extension == ".gltf") {
				assetType = AssetType::Mesh;
			}
	else if (extension == ".ttf" || extension == ".mtsdf") {
				assetType = AssetType::Font;
			}
			else if (extension == ".json") {
				assetType = AssetType::Json;
			}
			else if (extension == ".wav" || extension == ".mp3") {
				assetType = AssetType::Audio;
			}
			else if (extension == ".hlsl") {
				assetType = AssetType::Shader;
			}

	return assetType;
}

void szg::EditorAssetContentsCollector::collect_assets() {
	auto& instance = GetInstance();
	for (i32 i = 1; i < ASSET_ROOT_TYPE_MAX; ++i) {
		std::filesystem::path rootPath = std::filesystem::path(ROOT_PATH[i]);

		for (auto directory : std::filesystem::recursive_directory_iterator(rootPath)) {
			if (directory.is_directory()) {
				continue;
			}
			std::filesystem::path relativePath = std::filesystem::relative(directory.path(), rootPath);
			std::string extension = directory.path().extension().string();
			AssetType assetType = GetAssetTypeByExtension(extension);
			std::lock_guard<std::mutex> lock(instance.mutex);
			auto& assetMap = instance.assetMaps[static_cast<i32>(assetType)];
			std::string assetName = relativePath.filename().string();
			// 既に登録されている場合はスキップ
			if (assetMap.contains(assetName)) {
				continue;
			}

			// 登録
			AssetEntry entry;
			entry.type = assetType;
			entry.name = assetName;
			entry.extension = extension;
			entry.path = directory.path();
			entry.loadFunc = nullptr; // TODO: ロード関数の設定
			assetMap[assetName] = entry;
			
			// fbxとgltfはMeshだけでなくBoneとAnimationも登録する
			if (extension == ".fbx" || extension == ".gltf") {
				instance.assetMaps[static_cast<i32>(AssetType::Bone)][assetName] = entry;
				instance.assetMaps[static_cast<i32>(AssetType::Animation)][assetName] = entry;
			}
		}
	}
}
