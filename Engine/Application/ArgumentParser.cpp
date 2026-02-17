#include "ArgumentParser.h"

#include <sstream>

#include <windows.h>

#include <Library/Utility/Tools/ConvertString.h>

#include "Engine/Application/Logger.h"

using namespace szg;

void ArgumentParser::Parse() {
	ArgumentParser& instance = GetInstance();
	instance.arguments.clear();
	instance.argumentsInverseIndex.clear();

	wchar_t* commandLine = GetCommandLineW();

	// スペース区切りで格納
	std::wstringstream stream;
	stream << commandLine;
	i32 index = 0;
	while (!stream.eof()) {
		std::wstring argW;
		stream >> argW;
		if (argW.length() == 0) {
			continue;
		}
		std::string argA = ConvertString(argW);
		instance.arguments.emplace_back(argA);
		instance.argumentsInverseIndex[argA] = index;
		szgInformation("Command line argument [{}]: {}", index, argA);
		++index;
	}
}

bool ArgumentParser::Contains(const std::string& arg) {
	return GetInstance().argumentsInverseIndex.contains(arg);
}

const std::string& ArgumentParser::ValueByIndex(u64 index) {
	return GetInstance().arguments[index];
}

std::optional<u64> ArgumentParser::ArgIndexByValue(const std::string& value) {
	ArgumentParser& instance = GetInstance();
	if (!instance.argumentsInverseIndex.contains(value)) {
		return std::nullopt;
	}
	return instance.argumentsInverseIndex[value];
}

std::optional<std::string> ArgumentParser::FindValueStartWith(const std::string& prefix, u64 position) noexcept {
	ArgumentParser& instance = GetInstance();
	for (const std::string& arg : instance.arguments) {
		if (arg.starts_with(prefix)) {
			--position;
			if (position == 0) {
				return arg;
			}
		}
	}
	return std::nullopt;
}
