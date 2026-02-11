#pragma once

#include "Library/Utility/Template/SingletonInterface.h"

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace szg {

class SkeletonAsset;

/// <summary>
/// Skeletonアセット保持クラス(スレッドセーフ)
/// </summary>
class SkeletonLibrary final : SingletonInterface<SkeletonLibrary> {
	SZG_CLASS_SINGLETON(SkeletonLibrary)

public:
	/// <summary>
	/// ロードキューにファイルを追加
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	static void RegisterLoadQue(const std::filesystem::path& filePath);

	static void Unload(const std::string& name);

	/// <summary>
	/// Skeletonの取得
	/// </summary>
	/// <param name="name">Skeleton名</param>
	/// <returns>SkeletonResourceクラスのshared_ptr</returns>
	static std::shared_ptr<const SkeletonAsset> GetSkeleton(const std::string& name);

	/// <summary>
	/// Skeletonが登録されているか取得
	/// </summary>
	/// <param name="name">確認するSkeleton名</param>
	/// <returns>登録されていればtrue[bool]</returns>
	static bool IsRegistered(const std::string& name);

	/// <summary>
	/// nameとしてListに保存
	/// </summary>
	/// <param name="name">登録名</param>
	/// <param name="data">ロード済みデータ</param>
	static void Transfer(const std::string& name, std::shared_ptr<SkeletonAsset>& data);

private:
	/// <summary>
	/// Skeletonが登録されているか取得(mutexなし)
	/// </summary>
	/// <param name="name">確認するSkeleton名</param>
	/// <returns>登録されていればtrue[bool]</returns>
	static bool IsRegisteredNonlocking(const std::string& name);

private:
	std::unordered_map<std::string, std::shared_ptr<SkeletonAsset>> instanceList;

	static inline std::mutex mutex{};
};

}; // szg
