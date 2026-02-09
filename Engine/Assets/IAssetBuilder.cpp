#include "IAssetBuilder.h"

using namespace szg;

std::filesystem::path IAssetBuilder::ResolveFilePath(const std::filesystem::path& filePath, const std::string& subdirectory) {
	// 相対ディレクトリで始まる場合
	if (filePath.native().starts_with(L".\\") || filePath.native().starts_with(L"./")) {
		return filePath;
	}
	else if (filePath.native().starts_with(L"[[szg]]") || filePath.native().starts_with(L"[[SZG]]")) {
		// エンジンリソース
		std::filesystem::path result{ "./SyzygyEngine/EngineResources/" };
		if (!subdirectory.empty()) {
			result /= subdirectory;
		}
		return result / filePath.native().substr(8);
	}
#ifdef DEBUG_FEATURES_ENABLE
	else if (filePath.native().starts_with(L"[[editor]]") || filePath.native().starts_with(L"[[EDITOR]]")) {
		// エディターリソース
		std::filesystem::path result{ "./SyzygyEngine/EditorResources/" };
		if (!subdirectory.empty()) {
			result /= subdirectory;
		}
		return result / filePath.native().substr(11);
	}
#endif // DEBUG_FEATURES_ENABLE
	// ファイル名のみor一部ディレクトリの続きの場合
	else {
		std::filesystem::path result{ "./Game/Assets/" };
		if (!subdirectory.empty()) {
			result /= subdirectory;
		}
		return result / filePath;
	}
}
