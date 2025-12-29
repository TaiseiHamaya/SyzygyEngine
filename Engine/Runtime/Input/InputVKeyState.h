#pragma once

#include <unordered_set>

#include "InputEnum.h"

#include <Library/Utility/Template/SingletonInterface.h>

namespace szg {

class InputVKeyState final : public SingletonInterface<InputVKeyState> {
	SZG_CLASS_SINGLETON(InputVKeyState)

public:
	/// <summary>
	/// リセット処理
	/// </summary>
	static void BeginFrame() noexcept;

	/// <summary>
	/// WindowProcでWM_KEYDOWNを受け取ったときに呼ばれる
	/// </summary>
	/// <param name="c"></param>
	/// <param name="pram"></param>
	static void OnKeyDown(VirtualKeyID key, u32 pram);

	/// <summary>
	/// 仮想キーが入力されたかを取得
	/// </summary>
	/// <param name="keyId"></param>
	/// <returns></returns>
	static bool IsDownVKey(VirtualKeyID keyId) noexcept;

private:
	std::unordered_set<VirtualKeyID> virtualKeys;
};

}; // namespace szg
