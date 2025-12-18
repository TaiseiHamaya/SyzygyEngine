#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "IEditorCommand.h"

#include <functional>

#include <Library/Utility/Template/Reference.h>

namespace szg {

template<typename T>
class EditorValueChangeCommand final : public IEditorCommand {
public:
	EditorValueChangeCommand(Reference<T> target_);
	~EditorValueChangeCommand() = default;

public:
	void prepare();
	void execute() override;
	void undo() override;

private:
	Reference<T> target;
	T recent;
	T changed;
};

template<typename T>
EditorValueChangeCommand<T>::EditorValueChangeCommand(Reference<T> target_) {
	target = target_;
	recent = *target;
}

template<typename T>
void EditorValueChangeCommand<T>::prepare() {
	changed = *target;
}

template<typename T>
void EditorValueChangeCommand<T>::execute() {
	*target = changed;
}

template<typename T>
void EditorValueChangeCommand<T>::undo() {
	*target = recent;
}

template<typename T>
class EditorValueChangeCommandLambda final : public IEditorCommand {
public:
	EditorValueChangeCommandLambda(std::function<T&()> function_);
	~EditorValueChangeCommandLambda() = default;

public:
	void prepare();
	void execute() override;
	void undo() override;

private:
	std::function<T& ()> valueFunc;
	T recent;
	T changed;
};

template<typename T>
EditorValueChangeCommandLambda<T>::EditorValueChangeCommandLambda(std::function<T& ()> function_) {
	valueFunc = function_;
	recent = valueFunc();
}

template<typename T>
void EditorValueChangeCommandLambda<T>::prepare() {
	changed = valueFunc();
}

template<typename T>
void EditorValueChangeCommandLambda<T>::execute() {
	valueFunc() = changed;
}

template<typename T>
void EditorValueChangeCommandLambda<T>::undo() {
	valueFunc() = recent;
}

}; // szg

#endif // DEBUG_FEATURES_ENABLE
