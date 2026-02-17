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

Reference<const std::any> RuntimeStorage::GetValueImm(const std::string& groupName, const std::string& valueName) {
	auto& instance = GetInstance();
	if (!instance.runtimeValues.contains(groupName)) {
		return nullptr;
	}
	auto& valueGroup = instance.runtimeValues.at(groupName);
	if (!valueGroup.contains(valueName)) {
		return nullptr;
	}
	return valueGroup.at(valueName);
}

Reference<std::any> RuntimeStorage::GetValueMut(const std::string& groupName, const std::string& valueName) {
	auto& instance = GetInstance();
	if (!instance.runtimeValues.contains(groupName)) {
		return nullptr;
	}
	auto& valueGroup = instance.runtimeValues.at(groupName);
	if (!valueGroup.contains(valueName)) {
		return nullptr;
	}
	return valueGroup.at(valueName);
}
