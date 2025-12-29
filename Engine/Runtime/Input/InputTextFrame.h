#pragma once

#include <string>

#include <Library/Utility/Template/SingletonInterface.h>

namespace szg {

class InputTextFrame final : public SingletonInterface<InputTextFrame> {
	SZG_CLASS_SINGLETON(InputTextFrame)

public:
	/// <summary>
	/// リセット処理
	/// </summary>
	static void BeginFrame() noexcept;

	/// <summary>
	/// WindowProcでWM_CHARを受け取ったときに呼ばれる
	/// </summary>
	/// <param name="c"></param>
	/// <param name="pram"></param>
	static void ProssesInputChar(wchar_t c, u32 pram);

	/// <summary>
	/// IMEに変換終了通知が来た場合に呼ばれる
	/// </summary>
	static void SetImmEnter();

	/// <summary>
	/// フレームで入力されたテキストを取得
	/// </summary>
	/// <returns></returns>
	static const std::wstring& FrameInputTextImm() noexcept;

	/// <summary>
	/// IMEの変換終了があったかを取得
	/// </summary>
	/// <returns></returns>
	static bool IsImmEnter();

private:
	bool isImmEnter{ false };
	std::wstring frameInputBuffer;
};

} // namespace szg
