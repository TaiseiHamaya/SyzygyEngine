#pragma once

#include <string>

#include <Library/Utility/Template/SingletonInterface.h>

namespace szg {

class TextInput final : public SingletonInterface<TextInput> {
	SZG_CLASS_SINGLETON(TextInput)

public:
	static void BeginFrame() noexcept;
	static void ProssesInput(wchar_t c, u32 pram);

	static const std::wstring& FrameInputImm() noexcept;

private:
	std::wstring frameInputBuffer;
};

} // namespace szg
