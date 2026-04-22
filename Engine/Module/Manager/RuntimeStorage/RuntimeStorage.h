#pragma once

#include <any>
#include <optional>
#include <string>
#include <unordered_map>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Template/SingletonInterface.h>

namespace szg {

class RuntimeStorage : public SingletonInterface<RuntimeStorage> {
	SZG_CLASS_SINGLETON(RuntimeStorage)

public:
	using ValueGroup = std::unordered_map<std::string, std::any>;

public:
	static ValueGroup& GetValueList(const std::string& name);

	template<typename T>
	static std::optional<const T> GetValueImm(const std::string& groupName, const std::string& valueName);

	template<typename T>
	static std::optional<T> GetValueMut(const std::string& groupName, const std::string& valueName);

private:
	std::unordered_map<std::string, ValueGroup> runtimeValues;
};

template<typename T>
inline std::optional<const T> RuntimeStorage::GetValueImm(const std::string& groupName, const std::string& valueName) {
	return GetValueMut<const T>(groupName, valueName);
}

template<typename T>
std::optional<T> RuntimeStorage::GetValueMut(const std::string& groupName, const std::string& valueName) {
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
