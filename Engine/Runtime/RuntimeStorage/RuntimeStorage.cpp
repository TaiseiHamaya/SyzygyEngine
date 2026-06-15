#include "RuntimeStorage.h"

#include "Engine/Application/Logger.h"

using namespace szg;

RuntimeStorage::ValueGroup& RuntimeStorage::GetValueList(const std::string& name) {
	auto& instance = GetInstance();
	if (!instance.runtimeValues.contains(name)) {
		szgInformation("Create value group. Name-\'{}\'", name);
	}
	return instance.runtimeValues[name];
}
