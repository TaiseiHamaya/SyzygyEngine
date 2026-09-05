#pragma once

#include <filesystem>
#include <string_view>

#include "Library/Utility/Tools/ConstructorMacro.h"

/// <summary>
/// アセットビルダーインターフェース
/// </summary>
namespace szg {

class IAssetBuilder {
public:
	IAssetBuilder() = default;
	virtual ~IAssetBuilder() = default;

	SZG_CLASS_DELETED(IAssetBuilder)

public:
	/// <summary>
	/// ロード実行
	/// </summary>
	/// <returns>Succeed / Failed</returns>
	virtual bool run() = 0;

	/// <summary>
	/// transfer直前に呼ばれる
	/// </summary>
	virtual void postprocess() = 0;

	/// <summary>
	/// AssetLibraryへ転送
	/// </summary>
	virtual void transfer() = 0;

public:
	/// <summary>
	/// <para>接頭辞付きファイルパスを解決する</para>
	/// <para>[[szg]] / [[SZG]] : エンジンリソースディレクトリ</para>
	/// <para>"./"    / ".\"    : exeからの相対ディレクトリ</para>
	/// <para>その他            : リソースディレクトリ</para>
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <param name="subdirectory">リソースディレクトリ直下の</param>
	/// <returns></returns>
	static std::filesystem::path ResolveFilePath(const std::filesystem::path& filePath, std::string_view subdirectory = "");

protected:
	std::filesystem::path filePath;
};

}; // szg
