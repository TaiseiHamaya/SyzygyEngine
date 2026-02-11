#include "FontAtlasMSDFLibrary.h"

using namespace szg;

#include <mutex>
#include <ranges>

#include <Library/Utility/Tools/SmartPointer.h>

#include "./FontAtlasMSDFBuilder.h"
#include "Engine/Application/Logger.h"
#include "Engine/Assets/BackgroundLoader/BackgroundLoader.h"

void FontAtlasMSDFLibrary::Initialize() {
	GetInstance();
}

void FontAtlasMSDFLibrary::Finalize() {
	std::lock_guard lock{ GetInstance().mutex };
	GetInstance().fontAtlases.clear();
}

void FontAtlasMSDFLibrary::RegisterLoadQue(const std::filesystem::path& filePath) {
	if (IsRegistered(filePath.filename().string())) {
		return;
	}
	// BackgroundLoaderにイベント送信
	BackgroundLoader::RegisterLoadQue(
		eps::CreateUnique<FontAtlasMSDFBuilder>(filePath)
	);
}

std::shared_ptr<const FontAtlasMSDFAsset> FontAtlasMSDFLibrary::Get(const std::string& name) noexcept(false) {
	auto& instance = GetInstance();
	std::lock_guard lock{ instance.mutex };
	if (IsRegisteredNonlocking(name)) {
		return instance.fontAtlases.at(name);
	}
	else {
		szgWarning("FontAtlasMSDF '{}' is not registered.", name);
		return nullptr;
	}
}

bool FontAtlasMSDFLibrary::IsRegistered(const std::string& name) noexcept(false) {
	auto& instance = GetInstance();
	std::lock_guard lock{ instance.mutex };
	return IsRegisteredNonlocking(name);
}

void FontAtlasMSDFLibrary::Unload(const std::string& name) {
	std::lock_guard lock{ GetInstance().mutex };
	if (IsRegisteredNonlocking(name)) {
		GetInstance().fontAtlases.erase(name);
	}
}

void FontAtlasMSDFLibrary::Transfer(const std::string& name, std::shared_ptr<FontAtlasMSDFAsset>& fontAtlas) {
	auto& instance = GetInstance();
	std::lock_guard lock{ instance.mutex };
	if (IsRegisteredNonlocking(name)) {
		return;
	}
	instance.fontAtlases.emplace(name, fontAtlas);
}

bool FontAtlasMSDFLibrary::IsRegisteredNonlocking(const std::string& name) noexcept(false) {
	auto& instance = GetInstance();
	return instance.fontAtlases.contains(name);
}
