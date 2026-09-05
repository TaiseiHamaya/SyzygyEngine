#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Template/SingletonInterface.h>

#include <concepts>
#include <functional>
#include <type_traits>

#include "EditorCommandInvoker.h"
#include "EditorValueChangeCommand.h"

namespace szg {

template<typename Member, typename Container, typename Accessor = DefaultAccessor<Container>>
struct AccessorValueType {
	using type = std::remove_cvref_t<
		std::invoke_result_t<Member, decltype(Accessor::Get(std::declval<Container&>(), std::declval<i32>()))>
	>;
};

template<typename Member, typename Container, typename Accessor = DefaultAccessor<Container>>
using AccessorValueType_t = AccessorValueType<Member, Container, Accessor>::type;

template<typename Container>
struct DefaultAccessor {
	inline static auto& Get(Container& c, i64 index) { return c.at(index); }
};

template<typename T>
struct DefaultAccessor<std::list<T>> {
	inline static auto& Get(std::list<T>& c, i64 index) {
		return *std::advance(c.begin(), index);
	}
};

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

	template<typename Member, typename Container, typename Accessor = DefaultAccessor<Container>, typename T = AccessorValueType_t<Member, Container, Accessor>>
		requires std::copyable<T>
	static void GenCommandInstant(Container& container, i32 index, Member member, std::type_identity_t<const T&> value = {});

	template<typename Member, typename Container, typename Accessor = DefaultAccessor<Container>, typename T = AccessorValueType_t<Member, Container, Accessor>>
		requires std::copyable<T>
	static void GenCommand(Container& container, i32 index, Member member);
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

template<typename Member, typename Container, typename Accessor, typename T>
	requires std::copyable<T>
inline void EditorValueChangeCommandHandler::GenCommandInstant(Container& container, i32 index, Member member, std::type_identity_t<const T&> value) {
	GenCommand<Member, Container, Accessor>(container, index, member);
	std::invoke(member, Accessor::Get(container, index)) = value;
	End();
};

template<typename Member, typename Container, typename Accessor, typename T>
	requires std::copyable<T>
void EditorValueChangeCommandHandler::GenCommand(Container& container, i32 index, Member member) {
	auto lambda = [&container, index, member]() -> T& {
		return std::invoke(member, Accessor::Get(container, index));
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
