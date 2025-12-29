#pragma once

#include <string>

#include <Library/Math/Vector2.h>
#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class StringRectInstance;

class InputTextController final {
public:
	InputTextController() = default;
	~InputTextController() = default;

	InputTextController(i32 maxLength_);
	InputTextController(const std::wstring& initString);
	InputTextController(const std::wstring& initString, i32 maxLength_);

	SZG_CLASS_MOVE_ONLY(InputTextController)

public:
	void initialize();
	void update();

public:
	void start_input(const Vector2& screenPosition);
	void end_input();

public:
	void set_text(const std::wstring& newText);
	const std::wstring& text_imm() const noexcept;

	void set_max_length(i32 newMaxLength) noexcept;

	void clear() noexcept;

	r32 calculate_cursor_offset(Reference<szg::StringRectInstance> stringRect) const;

	bool is_return() const noexcept;

	bool is_inputting() const noexcept;

private:
	void process_control_key();

private:
	bool isInputting{ false };

	i32 cursorPos{ 0 };
	i32 maxLength{ 0 };

	std::wstring text;
};

}
