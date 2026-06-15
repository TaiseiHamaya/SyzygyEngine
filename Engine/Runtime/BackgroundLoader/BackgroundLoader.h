#pragma once

#include <list>
#include <memory>
#include <thread>

#include "Engine/Assets/IAssetBuilder.h"

#include <Library/Utility/Template/SingletonInterface.h>

/// <summary>
/// バックグラウンドロード用クラス(スレッドセーフ)
/// </summary>
namespace szg {

class BackgroundLoader final : public SingletonInterface<BackgroundLoader> {
	SZG_CLASS_SINGLETON(BackgroundLoader)

public:
	/// <summary>
	/// 初期化
	/// </summary>
	static void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	static void Finalize();

	/// <summary>
	/// ロードイベントの登録
	/// </summary>
	/// <param name="builder"></param>
	static void RegisterLoadQue(std::unique_ptr<IAssetBuilder> builder) noexcept(false);

	/// <summary>
	/// ロードが完了するまで待機
	/// </summary>
	static void WaitEndExecute();

	/// <summary>
	/// ロード中か取得
	/// </summary>
	/// <returns></returns>
	static bool IsLoading() noexcept;

private:
	/// <summary>
	/// 初期化
	/// </summary>
	void initialize() noexcept(false);

	/// <summary>
	/// ロード管理用関数
	/// </summary>
	void load_manager();

	/// <summary>
	/// 読み込み済みテクスチャのビューを作成
	/// </summary>
	void postprocess();

	/// <summary>
	/// ロード済みデータを各Managerに転送
	/// </summary>
	void transfer_all();

private:

	/// <summary>
	/// イベントデータ
	/// </summary>
	struct EventList {
		std::unique_ptr<IAssetBuilder> data; // 実データ
	};

	/// <summary>
	/// ロードスレッド
	/// </summary>
	std::thread loadFunc;

	/// <summary>
	/// イベント一覧
	/// </summary>
	std::list<EventList> loadEvents;

	/// <summary>
	/// ロードが終わって転送待ち
	/// </summary>
	std::list<EventList> waitTransferQue;

	/// <summary>
	/// マルチスレッド終了判定用
	/// </summary>
	bool isEndProgram{ false };

	/// <summary>
	/// ロード中フラグ
	/// </summary>
	bool isLoading{ false };
};

}; // szg
