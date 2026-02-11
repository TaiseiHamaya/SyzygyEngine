#include "SkeletonLibrary.h"

using namespace szg;

#include <mutex>

#include "./SkeletonAsset.h"
#include "./SkeletonAssetBuilder.h"
#include "Engine/Application/Logger.h"
#include "Engine/Assets/BackgroundLoader/BackgroundLoader.h"

#include <Library/Utility/Tools/SmartPointer.h>

#ifdef DEBUG_FEATURES_ENABLE
#include <imgui.h>
#endif // _DEBUG

std::mutex skeletonMutex;

void SkeletonLibrary::RegisterLoadQue(const std::filesystem::path& filePath) {
	// ロード済みの場合は何もしない
	if (IsRegistered(filePath.filename().string())) {
		return;
	}
	// BackgroundLoaderにイベント送信
	BackgroundLoader::RegisterLoadQue(eps::CreateUnique<SkeletonAssetBuilder>(filePath));
}

std::shared_ptr<const SkeletonAsset> SkeletonLibrary::GetSkeleton(const std::string& name) {
	std::lock_guard<std::mutex> lock{ skeletonMutex };
	if (IsRegisteredNonlocking(name)) {
		return GetInstance().instanceList.at(name);
	}
	else {
		szgWarning("Unloading skeleton. Name-\'{:}\'", name);
		return nullptr;
	}
}

bool SkeletonLibrary::IsRegistered(const std::string& name) {
	std::lock_guard<std::mutex> lock{ skeletonMutex };
	return IsRegisteredNonlocking(name);

}

void SkeletonLibrary::Transfer(const std::string& name, std::shared_ptr<SkeletonAsset>& data) {
	std::lock_guard<std::mutex> lock{ skeletonMutex };
	szgInformation("Transfer new Skeleton. Name-\'{:}\', Address-\'{:016}\'", name, (void*)data.get());
	GetInstance().instanceList.emplace(name, data);
}

bool SkeletonLibrary::IsRegisteredNonlocking(const std::string& name) {
	return GetInstance().instanceList.contains(name);
}
