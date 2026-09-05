#pragma once

#include <Library/Utility/SparseSet.h>

#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"

namespace szg {

template<typename T>
struct DefaultAccessor<SparseSet<T>> {
	inline static auto& Get(SparseSet<T>& c, i64 index) { return *c.at_mut(index); }
};

}
