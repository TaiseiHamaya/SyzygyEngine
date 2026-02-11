#include "NodeAnimationLibrary.h"

using namespace szg;

#include <mutex>

#include <Library/Utility/Tools/SmartPointer.h>

#include "./NodeAnimationAsset.h"
#include "./NodeAnimationAssetBuilder.h"
#include "Engine/Application/Logger.h"
#include "Engine/Assets/BackgroundLoader/BackgroundLoader.h"

void NodeAnimationLibrary::RegisterLoadQue(const std::filesystem::path& filePath) {
	// ロード済みの場合は何もしない
	if (IsRegistered(filePath.filename().string())) {
		return;
	}
	// BackgroundLoaderにイベント送信
	BackgroundLoader::RegisterLoadQue(eps::CreateUnique<NodeAnimationAssetBuilder>(filePath));
}

void szg::NodeAnimationLibrary::Unload(const std::string& name) {
	std::lock_guard<std::mutex> lock{ mutex };
	auto& instance = GetInstance();
	if (IsRegisteredNonlocking(name)) {
		szgInformation("Unload NodeAnimation Name-\'{:}\'.", name);
		instance.instanceList.erase(name);
	}
}

std::shared_ptr<const NodeAnimationAsset> NodeAnimationLibrary::GetAnimation(const std::string& name) {
	std::lock_guard<std::mutex> lock{ mutex };
	if (IsRegisteredNonlocking(name)) {
		return GetInstance().instanceList.at(name);
	}
	else {
		//Console("Unloading . Name-\'{:}\'", name);
		return nullptr;
	}
}

bool NodeAnimationLibrary::IsRegistered(const std::string& name) {
	std::lock_guard<std::mutex> lock{ mutex };
	return IsRegisteredNonlocking(name);
}

void NodeAnimationLibrary::Transfer(const std::string& name, std::shared_ptr<NodeAnimationAsset> data) {
	std::lock_guard<std::mutex> lock{ mutex };
	szgInformation("Transfer new NodeAnimation. Name-\'{:}\', Address-\'{:016}\'", name, (void*)data.get());
	GetInstance().instanceList.emplace(name, data);
}

bool NodeAnimationLibrary::IsRegisteredNonlocking(const std::string& name) {
	return GetInstance().instanceList.contains(name);
}
