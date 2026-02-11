#pragma once

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include <Library/Utility/Template/SingletonInterface.h>

namespace szg {

class PolygonMesh;

/// <summary>
/// メッシュアセット保持クラス(スレッドセーフ)
/// </summary>
class PolygonMeshLibrary final : public SingletonInterface<PolygonMeshLibrary> {
	SZG_CLASS_SINGLETON(PolygonMeshLibrary)

public:
	/// <summary>
	/// ロードキューに.objファイルを追加
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	static void RegisterLoadQue(const std::filesystem::path& filePath);

	static void Unload(const std::string& name);

	/// <summary>
	/// メッシュの取得
	/// </summary>
	/// <param name="meshName">メッシュ名</param>
	/// <returns>PolygonMeshクラスのweak_ptr</returns>
	static std::shared_ptr<const PolygonMesh> GetPolygonMesh(const std::string& meshName);

	/// <summary>
	/// メッシュが登録されているか取得
	/// </summary>
	/// <param name="meshName">確認するメッシュ名</param>
	/// <returns>登録されていればtrue[bool]</returns>
	static bool IsRegistered(const std::string& meshName);

	/// <summary>
	/// nameでPolygonMeshデータを登録済みデータに保存
	/// </summary>
	/// <param name="name">登録名</param>
	/// <param name="data">ロード済みデータ</param>
	static void Transfer(const std::string& name, std::shared_ptr<PolygonMesh>& data);

private:
	/// <summary>
	/// メッシュが登録されているか取得(mutexなし)
	/// </summary>
	/// <param name="meshName">確認するメッシュ名</param>
	/// <returns>登録されていればtrue[bool]</returns>
	static bool IsRegisteredNonlocking(const std::string& meshName);

private:
	std::unordered_map<std::string, std::shared_ptr<PolygonMesh>> meshInstanceList;

	static inline std::mutex mutex{};
};

}; // szg
