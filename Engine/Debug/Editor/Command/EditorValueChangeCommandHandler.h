#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Template/SingletonInterface.h>

#include <concepts>
#include <functional>
#include <vector>

#include "EditorCommandInvoker.h"
#include "EditorValueChangeCommand.h"

namespace szg {

class EditorValueChangeCommandHandler final : public SingletonInterface<EditorValueChangeCommandHandler> {
	SZG_CLASS_SINGLETON(EditorValueChangeCommandHandler)

private:
	static void Start(std::function<void(void)> endCallFunc);

public:
	static void End();

	static bool IsActive();

private:
	std::function<void(void)> endCallFunc{};

public:
	template<typename T>
		requires std::copyable<T>
	static void GenCommandInstant(Reference<T> target, const T& value = {});
	
	template<typename T>
		requires std::copyable<T>
	static void GenCommand(Reference<T> target);

	template<typename T, typename Struct>
		requires std::copyable<T>
	static void GenCommandInstant(std::vector<Struct>& container, i32 index, T Struct::* member, const T& value = {});

	template<typename T, typename Struct>
		requires std::copyable<T>
	static void GenCommand(std::vector<Struct>& container, i32 index, T Struct::* member);
};

template<typename T>
	requires std::copyable<T>
inline void EditorValueChangeCommandHandler::GenCommandInstant(Reference<T> target, const T& value) {
	GenCommand<T>(target);
	*target = value;
	End();
}

template<typename T>
	requires std::copyable<T>
void EditorValueChangeCommandHandler::GenCommand(Reference<T> target) {
	Start([target]() {
		std::unique_ptr<EditorValueChangeCommand<T>> command = std::make_unique<EditorValueChangeCommand<T>>(target);
		command->prepare();
		EditorCommandInvoker::Execute(
			std::move(command)
		);
	});
}

template<typename T, typename Struct>
	requires std::copyable<T>
inline void EditorValueChangeCommandHandler::GenCommandInstant(std::vector<Struct>& container, i32 index, T Struct::* member, const T& value) {
	GenCommand(container, index, member);
	container.at(index).*member = value;
	End();
};

template<typename T, typename Struct>
	requires std::copyable<T>
void EditorValueChangeCommandHandler::GenCommand(std::vector<Struct>& container, i32 index, T Struct::* member) {
	auto lambda = [&container, index, member]() -> T& {
		return container.at(index).*member;
	};

	Start([lambda]() {
		std::unique_ptr<EditorValueChangeCommandLambda<T>> command = std::make_unique<EditorValueChangeCommandLambda<T>>(lambda);
		command->prepare();
		EditorCommandInvoker::Execute(
			std::move(command)
		);
	});
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
