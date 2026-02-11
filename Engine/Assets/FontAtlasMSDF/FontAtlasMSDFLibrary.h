#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

#include <Library/Utility/Template/SingletonInterface.h>

namespace szg {

class FontAtlasMSDFAsset;

class FontAtlasMSDFLibrary final : public SingletonInterface<FontAtlasMSDFLibrary> {
	SZG_CLASS_SINGLETON(FontAtlasMSDFLibrary)

public:
	static void Initialize();
	static void Finalize();

	static void RegisterLoadQue(const std::filesystem::path& filepath);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="name">拡張子付きファイル名</param>
	/// <returns>Textureクラスのweak_ptr</returns>
	static std::shared_ptr<const FontAtlasMSDFAsset> Get(const std::string& name) noexcept(false);

	/// <summary>
	/// テクスチャが登録済みか取得
	/// </summary>
	/// <param name="name">拡張子付きファイル名</param>
	/// <returns>bool値</returns>
	static bool IsRegistered(const std::string& name) noexcept(false);

	/// <summary>
	/// 読み込み済みテクスチャのアンロード
	/// </summary>
	/// <param name="name">テクスチャ名</param>
	static void Unload(const std::string& name);

	/// <summary>
	/// 転送[ユーザー使用は基本しないこと]
	/// </summary>
	/// <param name="name">転送時の名前</param>
	/// <param name="data">転送データ</param>
	static void Transfer(const std::string& name, std::shared_ptr<FontAtlasMSDFAsset>& data);

private:
	/// <summary>
	/// テクスチャが登録済みか取得(mutexなし)
	/// </summary>
	/// <param name="name">テクスチャ名</param>
	/// <returns>bool値</returns>
	static bool IsRegisteredNonlocking(const std::string& name) noexcept(false);

private:
	std::unordered_map<std::string, std::shared_ptr<const FontAtlasMSDFAsset>> fontAtlases;
	mutable std::mutex mutex;
};

}; // szg
