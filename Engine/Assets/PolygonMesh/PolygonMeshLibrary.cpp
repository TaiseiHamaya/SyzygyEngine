#include "PolygonMeshLibrary.h"

using namespace szg;

#include <Library/Utility/Tools/SmartPointer.h>

#include "./PolygonMesh.h"
#include "./PolygonMeshBuilder.h"
#include "Engine/Application/Logger.h"
#include "Engine/Assets/BackgroundLoader/BackgroundLoader.h"

#ifdef DEBUG_FEATURES_ENABLE
#include <imgui.h>
#endif // _DEBUG

void PolygonMeshLibrary::RegisterLoadQue(const std::filesystem::path& filePath) {
	// ロード済みの場合は何もしない
	if (IsRegistered(filePath.filename().string())) {
		return;
	}
	// BackgroundLoaderにイベント送信
	BackgroundLoader::RegisterLoadQue(
		eps::CreateUnique<PolygonMeshBuilder>(filePath)
	);
}

void szg::PolygonMeshLibrary::Unload(const std::string& name) {
	auto& instance = GetInstance();
	std::lock_guard<std::mutex> lock{ mutex };
	if (IsRegisteredNonlocking(name)) {
		szgInformation("Unload texture Name-\'{:}\'.", name);
		instance.meshInstanceList.erase(name);
	}
}

std::shared_ptr<const PolygonMesh> PolygonMeshLibrary::GetPolygonMesh(const std::string& meshName) {
	std::lock_guard<std::mutex> lock{ mutex };
	if (IsRegisteredNonlocking(meshName)) {
		return GetInstance().meshInstanceList.at(meshName);
	}
	else {
		// 存在しないメッシュを呼び出したらエラー用メッシュを使用する
		szgWarning("Unknown polygon mesh. Name-\'{:}\'", meshName);
		return GetInstance().meshInstanceList.at("ErrorObject.obj");
	}
}

bool PolygonMeshLibrary::IsRegistered(const std::string& meshName) {
	std::lock_guard<std::mutex> lock{ mutex };
	return IsRegisteredNonlocking(meshName);
}

void PolygonMeshLibrary::Transfer(const std::string& name, std::shared_ptr<PolygonMesh>& data) {
	std::lock_guard<std::mutex> lock{ mutex };
	szgInformation("Transfer new PolygonMesh. Name-\'{:}\', Address-\'{:016}\'", name, (void*)data.get());
	GetInstance().meshInstanceList.emplace(name, data);
}

bool PolygonMeshLibrary::IsRegisteredNonlocking(const std::string& meshName) {
	return GetInstance().meshInstanceList.contains(meshName);
}
