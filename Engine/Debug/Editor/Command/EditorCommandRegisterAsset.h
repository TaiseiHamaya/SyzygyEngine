#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <filesystem>

#include "./IEditorCommand.h"
#include "Engine/Assets/AssetTypeEnum.h"

namespace szg {

class EditorCommandRegisterAsset final : public IEditorCommand {
public:
	EditorCommandRegisterAsset(AssetType type, const std::filesystem::path& assetFilePath, bool isLazy = false);
	~EditorCommandRegisterAsset() override = default;

public:
	void execute() override;
	void undo() override;

private:
	AssetType type{ AssetType::Unknown };
	std::filesystem::path assetFilePath{};
	bool isLazy{ false };
};

class EditorCommandUnregisterAsset final : public IEditorCommand {
public:
	EditorCommandUnregisterAsset(AssetType type, const std::filesystem::path& assetFilePath);
	~EditorCommandUnregisterAsset() override = default;

public:
	void execute() override;
	void undo() override;

private:
	AssetType type{ AssetType::Unknown };
	std::filesystem::path assetFilePath{};
	bool isLazy{ false };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
