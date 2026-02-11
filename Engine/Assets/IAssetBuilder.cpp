#include "IAssetBuilder.h"

using namespace szg;

#include "./AssetRootPath.h"

std::filesystem::path IAssetBuilder::ResolveFilePath(const std::filesystem::path& filePath, const std::string& subdirectory) {
	// 正規化
	auto path = filePath.lexically_normal();

	// 相対ディレクトリで始まる場合
	if (filePath.native().starts_with(L".\\") || filePath.native().starts_with(L"./")) {
		return path;
	}
	else {
		for (i32 i = 1; i < ASSET_ROOT_TYPE_MAX; ++i) {
			// 接頭辞が一致する場合
			if (path.native().starts_with(ROOT_TAG_W[i])) {
				// 接頭辞を削除
				path = path.native().substr(std::wstring(ROOT_TAG_W[i]).length() + 1);
				// リソースディレクトリパスを付与
				auto basePath = std::filesystem::path(ROOT_PATH[i]);
				if (!subdirectory.empty()) {
					basePath /= subdirectory;
				}
				basePath /= path;
				return basePath;
			}
		}
	}

	// 識別子タグがない場合
	std::filesystem::path result{ "./Game/Assets/" };
	if (!subdirectory.empty()) {
		result /= subdirectory;
	}
	result /= filePath;
	return result;
}
