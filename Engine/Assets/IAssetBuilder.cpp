#include "IAssetBuilder.h"

#include "./AssetRootPath.h"

using namespace szg;

std::filesystem::path IAssetBuilder::ResolveFilePath(const std::filesystem::path& filePath, const std::string& subdirectory) {
	// 正規化
	auto path = filePath.lexically_normal();

	if (filePath.empty()) {
		return {};
	}
	else if (filePath.native()[0] == '[') {
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

	return filePath;
}
