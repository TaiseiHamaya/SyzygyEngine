#include "AudioLibrary.h"

using namespace szg;

#include <Library/Utility/Tools/SmartPointer.h>

#include "./AudioAsset.h"
#include "./AudioAssetBuilder.h"
#include "Engine/Application/Logger.h"
#include "Engine/Assets/BackgroundLoader/BackgroundLoader.h"

void AudioLibrary::Initialize() {
	// nullインスタンスの追加
	Transfer("NULL", nullptr);
}

void AudioLibrary::Finalize() {
	auto&& instance = GetInstance();
	instance.audioResources.clear();
}

void AudioLibrary::RegisterLoadQue(const std::filesystem::path& filePath) {
	if (IsRegistered(filePath.filename().string())) {
		return;
	}
	// BackgroundLoaderにイベント送信
	BackgroundLoader::RegisterLoadQue(
		eps::CreateUnique<AudioAssetBuilder>(filePath)
	);
}

void szg::AudioLibrary::Unload(const std::string& name) {
	std::lock_guard<std::mutex> lock{ mutex };
	auto& instance = GetInstance();
	if (IsRegisteredNonlocking(name)) {
		szgInformation("Unload audio Name-\'{:}\'.", name);
		instance.audioResources.erase(name);
	}
}

const std::unique_ptr<AudioAsset>& AudioLibrary::GetAudio(const std::string& audioName) {
	std::lock_guard<std::mutex> lock{ mutex };
	// 見つかったらそのデータのweak_ptrを返す
	if (IsRegisteredNonlocking(audioName)) {
		return GetInstance().audioResources.at(audioName);
	}
	else {
		szgWarning("Audio Name-\'{:}\' is not loading.", audioName);
		return GetInstance().audioResources.at("NULL");
	}
}

bool AudioLibrary::IsRegistered(const std::string& audioName) noexcept(false) {
	std::lock_guard<std::mutex> lock{ mutex };
	return IsRegisteredNonlocking(audioName);
}

void AudioLibrary::UnloadAudio(const std::string& audioName) {
	std::lock_guard<std::mutex> lock{ mutex };
	if (IsRegisteredNonlocking(audioName)) {
		GetInstance().audioResources.erase(audioName);
	}
}

void AudioLibrary::Transfer(const std::string& name, std::unique_ptr<AudioAsset>&& data) {
	std::lock_guard<std::mutex> lock{ mutex };
	if (IsRegisteredNonlocking(name)) {
		szgWarning("Transferring registered Audio. Name-\'{:}\', Address-\'{:016}\'", name, (void*)data.get());
		return;
	}
	szgInformation("Transfer new Audio. Name-\'{:}\', Address-\'{:016}\'", name, (void*)data.get());
	GetInstance().audioResources.emplace(name, std::move(data));

}

bool AudioLibrary::IsRegisteredNonlocking(const std::string& audioName) noexcept(false) {
	return GetInstance().audioResources.contains(audioName);
}
