#include "PrimitiveGeometryLibrary.h"

#include <mutex>

#include <Library/Utility/Tools/SmartPointer.h>

#include "./PrimitiveGeometryAsset.h"
#include "Engine/Application/Logger.h"
#include "Engine/Runtime/BackgroundLoader/BackgroundLoader.h"

#ifdef DEBUG_FEATURES_ENABLE
#include <imgui.h>
#endif // _DEBUG

std::mutex primitiveGeometryMutex;

using namespace szg;

std::shared_ptr<const PrimitiveGeometryAsset> PrimitiveGeometryLibrary::GetPrimitiveGeometry(const std::string& name) {
	std::lock_guard<std::mutex> lock{ primitiveGeometryMutex };
	if (IsRegisteredNonlocking(name)) {
		return GetInstance().primitiveGeometryList.at(name);
	}
	else {
		//Console("Unloading . Name-\'{:}\'", name);
		return nullptr;
	}
}

bool PrimitiveGeometryLibrary::IsRegistered(const std::string& name) {
	std::lock_guard<std::mutex> lock{ primitiveGeometryMutex };
	return IsRegisteredNonlocking(name);
}

void PrimitiveGeometryLibrary::Transfer(const std::string& name, std::shared_ptr<PrimitiveGeometryAsset> data) {
	std::lock_guard<std::mutex> lock{ primitiveGeometryMutex };
	szgInformation("Transfer new LineGroup. Name-\'{:}\', Address-\'{:016}\'", name, (void*)data.get());
	GetInstance().primitiveGeometryList.emplace(name, data);
}

bool PrimitiveGeometryLibrary::IsRegisteredNonlocking(const std::string& name) {
	return GetInstance().primitiveGeometryList.contains(name);
}
