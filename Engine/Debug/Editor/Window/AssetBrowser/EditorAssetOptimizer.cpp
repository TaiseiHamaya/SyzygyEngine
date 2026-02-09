#include "EditorAssetOptimizer.h"

#include <cctype>

#include "./Optimizer/FontAtlas/FontAtlasBuilder.h"
#include "./Optimizer/Texture/TextureOptimizer.h"

using namespace szg;

void szg::EditorAssetOptimizer::optimize_asset(const std::filesystem::path& inputFile, const std::filesystem::path& outputPath) {
	std::string extension = inputFile.extension().string();
	std::ranges::transform(extension, extension.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

	if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".tga" || extension == ".bmp") {
		TextureOptimizer{}.convert_wic_to_dds(inputFile, outputPath);
	}
	else if (extension == ".ttf") {
		FontAtlasBuilder{}.entry_point(inputFile, outputPath);
	}
}

bool szg::EditorAssetOptimizer::is_support_optimization(const std::filesystem::path& extension) const {
	std::string ext = extension.string();
	std::ranges::transform(ext, ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return
		(ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".tga" || ext == ".bmp") ||
		(ext == ".ttf");
}
