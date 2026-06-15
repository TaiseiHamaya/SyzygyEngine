#include "ShaderLibrary.h"

#include <mutex>

#include <Library/Utility/Tools/SmartPointer.h>

#include "./ShaderAsset.h"
#include "./ShaderAssetBuilder.h"
#include "Engine/Application/Logger.h"
#include "Engine/Runtime/BackgroundLoader/BackgroundLoader.h"

std::mutex shaderLibraryMutex;

using namespace szg;

void ShaderLibrary::RegisterLoadQue(const std::filesystem::path& filePath) {
	// ロード済みの場合は何もしない
	if (IsRegistered(filePath.filename().string())) {
		return;
	}
	// BackgroundLoaderにイベント送信
	BackgroundLoader::RegisterLoadQue(
		eps::CreateUnique<ShaderAssetBuilder>(filePath)
	);
}

std::shared_ptr<const ShaderAsset> ShaderLibrary::GetShader(const std::string& name) {
	std::lock_guard<std::mutex> lock{ shaderLibraryMutex };
	if (IsRegisteredNonlocking(name)) {
		return GetInstance().assets.at(name);
	}
	else {
		szgWarning("Unknown shader. Name-\'{:}\'", name);
		return nullptr;
	}
}

bool ShaderLibrary::IsRegistered(const std::string& meshName) {
	std::lock_guard<std::mutex> lock{ shaderLibraryMutex };
	return IsRegisteredNonlocking(meshName);
}

void ShaderLibrary::Transfer(const std::string& name, std::shared_ptr<ShaderAsset>& data) {
	std::lock_guard<std::mutex> lock{ shaderLibraryMutex };
	szgInformation("Transfer new shader. Name-\'{:}\', Address-\'{:016}\'", name, (void*)data.get());
	GetInstance().assets.emplace(name, data);
}

bool ShaderLibrary::IsRegisteredNonlocking(const std::string& name) {
	return GetInstance().assets.contains(name);
}
