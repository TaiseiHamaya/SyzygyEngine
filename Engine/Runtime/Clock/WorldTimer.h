#pragma once

#include <Library/Utility/Tools/ConstructorMacro.h>

namespace szg {

class WorldTimer final {
public:
	WorldTimer(r32 time = 0.0f);
	~WorldTimer() noexcept = default;

	SZG_CLASS_DEFAULT(WorldTimer)

public:
	void ahead(r32 scale = 1.0f);
	void back(r32 scale = 1.0f);
	void set(r32 time);
	r32 time() const;

	/// <summary>
	/// このフレームでちょうどtimeを越えたか
	/// 既知の不具合: backで戻った場合に正しく動作しない
	/// </summary>
	/// <param name="time"></param>
	/// <returns></returns>
	bool just_crossed(r32 time) const noexcept;

public:
	operator r32() const noexcept { return timer; };

private:
	r32 timer{ 0 };
};

}; // szg
