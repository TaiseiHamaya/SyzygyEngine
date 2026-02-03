#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Template/SingletonInterface.h>

#include <concepts>
#include <functional>

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
	static void GenCommand(Reference<T> target);

	template<typename T>
		requires std::copyable<T>
	static void GenCommand(std::function<T&()> function);
};

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
};

template<typename T>
	requires std::copyable<T>
void EditorValueChangeCommandHandler::GenCommand(std::function<T& ()> function) {
	Start([function]() {
		std::unique_ptr<EditorValueChangeCommandLambda<T>> command = std::make_unique<EditorValueChangeCommandLambda<T>>(function);
		command->prepare();
		EditorCommandInvoker::Execute(
			std::move(command)
		);
	});
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
