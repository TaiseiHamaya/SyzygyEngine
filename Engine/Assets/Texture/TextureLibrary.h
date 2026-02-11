#pragma once

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace szg {

class TextureAsset;

class TextureLibrary final {
private:
	TextureLibrary();

public:
	~TextureLibrary() noexcept;

private:
	TextureLibrary(const TextureLibrary&) = delete;
	TextureLibrary& operator=(const TextureLibrary&) = delete;

public:
	static TextureLibrary& GetInstance() noexcept;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	static void Initialize() noexcept;

	/// <summary>
	/// 終了処理
	/// </summary>
	static void Finalize();

	/// <summary>
	/// ロードキューにファイルを追加
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	static void RegisterLoadQue(const std::filesystem::path& filePath);

	static void Unload(const std::string& name);

	/// <summary>
	/// テクスチャデータの取得
	/// </summary>
	/// <param name="textureName">拡張子付きファイル名</param>
	/// <returns>Textureクラスのweak_ptr</returns>
	static std::shared_ptr<const TextureAsset> GetTexture(const std::string& textureName) noexcept(false);

	/// <summary>
	/// テクスチャが登録済みか取得
	/// </summary>
	/// <param name="textureName">拡張子付きファイル名</param>
	/// <returns>bool値</returns>
	static bool IsRegistered(const std::string& textureName) noexcept(false);

	/// <summary>
	/// 転送[ユーザー使用は基本しないこと]
	/// </summary>
	/// <param name="name">転送時の名前</param>
	/// <param name="data">転送データ</param>
	static void Transfer(const std::string& name, std::shared_ptr<TextureAsset>& data);

private:
	/// <summary>
	/// テクスチャが登録済みか取得(mutexなし)
	/// </summary>
	/// <param name="textureName">テクスチャ名</param>
	/// <returns>bool値</returns>
	static bool IsRegisteredNonlocking(const std::string& textureName) noexcept(false);

private: // メンバ変数
	std::unordered_map<std::string, std::shared_ptr<TextureAsset>> textureInstanceList; // テクスチャリスト

	static inline std::mutex mutex{};
};


}; // szg
