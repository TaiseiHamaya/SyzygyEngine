#pragma once

#include "Library/Utility/Template/SingletonInterface.h"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace szg {

class NodeAnimationAsset;

/// <summary>
/// Animationアセット保持クラス(スレッドセーフ)
/// </summary>
class NodeAnimationLibrary final : SingletonInterface<NodeAnimationLibrary> {
	SZG_CLASS_SINGLETON(NodeAnimationLibrary)

public:
	/// <summary>
	/// ロードキューにファイルを追加
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	static void RegisterLoadQue(const std::filesystem::path& filePath);

	/// <summary>
	/// Animationの取得
	/// </summary>
	/// <param name="name">Animation名</param>
	/// <returns>NodeAnimationResourceクラスのshared_ptr</returns>
	static std::shared_ptr<const NodeAnimationAsset> GetAnimation(const std::string& name);

	/// <summary>
	/// Animationが登録されているか取得
	/// </summary>
	/// <param name="name">確認するAnimation名</param>
	/// <returns>登録されていればtrue[bool]</returns>
	static bool IsRegistered(const std::string& name);

	/// <summary>
	/// nameとしてListに保存
	/// </summary>
	/// <param name="name">登録名</param>
	/// <param name="data">ロード済みデータ</param>
	static void Transfer(const std::string& name, std::shared_ptr<NodeAnimationAsset> data);

private:
	/// <summary>
	/// Animationが登録されているか取得(mutexなし)
	/// </summary>
	/// <param name="name">確認するAnimation名</param>
	/// <returns>登録されていればtrue[bool]</returns>
	static bool IsRegisteredNonlocking(const std::string& name);

private:
	std::unordered_map<std::string, std::shared_ptr<NodeAnimationAsset>> instanceList;
};


}; // szg
