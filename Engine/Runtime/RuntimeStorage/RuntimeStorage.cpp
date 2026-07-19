#include "RuntimeStorage.h"

#include "Engine/Application/Logger.h"

using namespace szg;

void szg::RuntimeStorage::OverwirteValue(const std::string& groupName, const std::string& valueName, std::any value) {
	auto& instance = GetInstance();
	if (!instance.runtimeValues.contains(groupName)) {
		szgInformation("Create value group. Name-\'{}\'", groupName);
	}
	instance.runtimeValues[groupName].insert_or_assign(valueName, value);
}

void szg::RuntimeStorage::WriteValue(const std::string& groupName, const std::string& valueName, std::any value) {
	auto& instance = GetInstance();
	if (!instance.runtimeValues.contains(groupName)) {
		szgInformation("Create value group. Name-\'{}\'", groupName);
	}
	instance.runtimeValues[groupName].emplace(valueName, value);
}
