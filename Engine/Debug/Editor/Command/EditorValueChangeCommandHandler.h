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

	template<typename T, typename Struct, typename Proj = std::identity>
		requires std::copyable<T>
	static void GenCommandInstant(std::vector<Struct>& container, i32 index, Proj proj = {}, const T& value = {});

	template<typename T, typename Struct, typename Proj = std::identity>
		requires std::copyable<T>
	static void GenCommand(std::vector<Struct>& container, i32 index, Proj proj);
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
	Start([target, recent = *target]() {
		std::unique_ptr<EditorValueChangeCommand<T>> command = std::make_unique<EditorValueChangeCommand<T>>(target, recent);
		command->prepare();
		EditorCommandInvoker::Execute(
			std::move(command)
		);
	});
}

template<typename T, typename Struct, typename Proj>
	requires std::copyable<T>
inline void EditorValueChangeCommandHandler::GenCommandInstant(std::vector<Struct>& container, i32 index, Proj proj, const T& value) {
	GenCommand<T>(container, index, proj);
	std::invoke(proj, container.at(index)) = value;
	End();
};

template<typename T, typename Struct, typename Proj>
	requires std::copyable<T>
void EditorValueChangeCommandHandler::GenCommand(std::vector<Struct>& container, i32 index, Proj proj) {
	auto lambda = [&container, index, proj]() -> T& {
		return std::invoke(proj, container.at(index));
	};

	Start([lambda, recent = lambda()]() {
		std::unique_ptr<EditorValueChangeCommandLambda<T>> command = std::make_unique<EditorValueChangeCommandLambda<T>>(lambda, recent);
		command->prepare();
		EditorCommandInvoker::Execute(
			std::move(command)
		);
	});
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
