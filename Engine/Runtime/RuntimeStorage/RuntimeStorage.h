#pragma once

#include <any>
#include <optional>
#include <string>
#include <unordered_map>

#include <Library/Utility/Template/SingletonInterface.h>

namespace szg {

class RuntimeStorage : public SingletonInterface<RuntimeStorage> {
	SZG_CLASS_SINGLETON(RuntimeStorage)

public:
	using ValueGroup = std::unordered_map<std::string, std::any>;

public:
	static void OverwirteValue(const std::string& groupName, const std::string& valueName, std::any value);

	static void WriteValue(const std::string& groupName, const std::string& valueName, std::any value);

	template<typename T>
	static std::optional<T> GetValue(const std::string& groupName, const std::string& valueName);

private:
	std::unordered_map<std::string, ValueGroup> runtimeValues;
};

template<typename T>
std::optional<T> RuntimeStorage::GetValue(const std::string& groupName, const std::string& valueName) {
	auto& instance = GetInstance();
	if (!instance.runtimeValues.contains(groupName)) {
		return std::nullopt;
	}
	auto& valueGroup = instance.runtimeValues.at(groupName);
	if (!valueGroup.contains(valueName)) {
		return std::nullopt;
	}
	std::any value = valueGroup.at(valueName);
	if (!value.has_value()) {
		return std::nullopt;
	}
	if (value.type() == typeid(T)) {
		return std::any_cast<T>(value);
	}
	else {
		return std::nullopt;
	}
}

}; // szg
