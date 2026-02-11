#include "TextureLibrary.h"

using namespace szg;

#include <Library/Utility/Tools/SmartPointer.h>

#include "./TextureAsset.h"
#include "./TextureAssetBuilder.h"
#include "Engine/Application/Logger.h"
#include "Engine/Assets/BackgroundLoader/BackgroundLoader.h"

TextureLibrary::TextureLibrary() = default;

TextureLibrary::~TextureLibrary() noexcept = default;

TextureLibrary& TextureLibrary::GetInstance() noexcept {
	static TextureLibrary instance{};
	return instance;
}

void TextureLibrary::Initialize() noexcept {
	GetInstance();
}

void TextureLibrary::Finalize() {
	auto&& list = GetInstance().textureInstanceList;
	list.clear();
}

void TextureLibrary::RegisterLoadQue(const std::filesystem::path& filePath) {
	if (IsRegistered(filePath.filename().string())) {
		return;
	}
	// BackgroundLoaderにイベント送信
	BackgroundLoader::RegisterLoadQue(
		eps::CreateUnique<TextureAssetBuilder>(filePath)
	);
}

void szg::TextureLibrary::Unload(const std::string& name) {
	std::lock_guard<std::mutex> lock{ mutex };
	if (IsRegisteredNonlocking(name)) {
		szgInformation("Unload texture Name-\'{:}\'.", name);
		GetInstance().textureInstanceList.erase(name);
	}
}

std::shared_ptr<const TextureAsset> TextureLibrary::GetTexture(const std::string& textureName) noexcept(false) {
	std::lock_guard<std::mutex> lock{ mutex };
	// 見つかったらそのデータのweak_ptrを返す
	if (IsRegisteredNonlocking(textureName)) {
		return GetInstance().textureInstanceList.at(textureName);
	}
	else {
		szgWarning("Texture Name-\'{:}\' is not loading.", textureName);
		return GetInstance().textureInstanceList.at("Error.png");
	}
}

bool TextureLibrary::IsRegistered(const std::string& textureName) noexcept(false) {
	std::lock_guard<std::mutex> lock{ mutex };
	return IsRegisteredNonlocking(textureName);
}

void TextureLibrary::Transfer(const std::string& name, std::shared_ptr<TextureAsset>& data) {
	std::lock_guard<std::mutex> lock{ mutex };
	if (IsRegisteredNonlocking(name)) {
		szgWarning("Transferring registered texture. Name-\'{:}\', Address-\'{:016}\'", name, (void*)data.get());
		return;
	}
	szgInformation("Transfer new Texture. Name-\'{:}\', Address-\'{:016}\'", name, (void*)data.get());
	GetInstance().textureInstanceList.emplace(name, data);
}

bool TextureLibrary::IsRegisteredNonlocking(const std::string& textureName) noexcept(false) {
	return GetInstance().textureInstanceList.contains(textureName);
}
