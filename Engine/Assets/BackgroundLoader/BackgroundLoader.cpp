#include "BackgroundLoader.h"

#include <functional>
#include <mutex>

#include "Engine/Application/Logger.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"

std::mutex executeMutex;
std::mutex referenceMutex;
std::condition_variable waitConditionVariable;
std::condition_variable loadConditionVariable;

using namespace szg;

void BackgroundLoader::Initialize() {
	GetInstance().initialize();
}

void BackgroundLoader::Finalize() {
	auto& instance = GetInstance();
	instance.isEndProgram = true;
	if (instance.loadFunc.joinable()) {
		loadConditionVariable.notify_all();
		instance.loadFunc.join();
	}
}

void BackgroundLoader::RegisterLoadQue(std::unique_ptr<IAssetBuilder> builder) noexcept(false) {
	// mutexのlock
	std::lock_guard<std::mutex> lock{ referenceMutex };
	auto& instance = GetInstance();
	instance.isLoading = true;

	instance.loadEvents.emplace_back(std::move(builder));

	// 条件変数通知
	loadConditionVariable.notify_all();
}

void BackgroundLoader::WaitEndExecute() {
	std::unique_lock<std::mutex> uniqueLock{ executeMutex };
	// 実行が終わるまで待機
	waitConditionVariable.wait(uniqueLock, [] { return !IsLoading(); });
}

bool BackgroundLoader::IsLoading() noexcept {
	return GetInstance().isLoading;
}

void BackgroundLoader::initialize() noexcept(false) {
	isLoading = false;
	isEndProgram = false;
	// ロード用スレッド作成
	loadFunc = std::thread{ std::bind(&BackgroundLoader::load_manager, this) };
	SetThreadDescription(loadFunc.native_handle(), L"Load Thread");
}

void BackgroundLoader::load_manager() {
	SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, L"en-US", nullptr);
	// プログラム自体の終了が行われない限りは続ける
	while (!isEndProgram) {
		// 条件変数用mutex
		std::unique_lock<std::mutex> lock{ referenceMutex };
		// loadEventが空ではない or プログラム終了通知が来ているまでwait
		loadConditionVariable.wait(lock, [&] {return !loadEvents.empty() || isEndProgram; });

		// プログラム終了ならループを抜ける
		if (isEndProgram) {
			break;
		}

		// 先頭のイベントを取得(lock中なのでok)
		auto&& nowEvent = loadEvents.begin();
		// ここからはunlock
		lock.unlock();

		// ----- ロードプロセス -----
		bool result = false;
		result = nowEvent->data->run();

		// 成功した場合は転送キューにへ
		if (result) {
			// 先頭要素を転送キューに追加(内部要素のmoveなので、listそのものはmutex必要なし)
			waitTransferQue.emplace_back(std::move(*nowEvent));
		}

		// mutexの再ロック
		lock.lock();
		// listの先頭要素をpop
		loadEvents.pop_front();

		// 空だったら自動execute
		if (loadEvents.empty()) {
			szgInformation("Load events is empty. Start uploading texture.");
			// ----- GPUコマンドの実行 -----
			// コマンド実行
			DxCommand::ExecuteTextureCommand();
			// 終了まで待機
			DxCommand::WaitTextureCommand();
			// リセット
			DxCommand::ResetTextureCommand();
			szgInformation("Succeeded.");

			// ----- 実行済みを転送 -----
			// 直前にやる
			postprocess();
			// 転送
			transfer_all();
			// 実行状態をfalseに
			isLoading = false;
			// waitしているかもしれないので通知
			waitConditionVariable.notify_all();
		}
		// 自動ロック解除
	}
}

void BackgroundLoader::postprocess() {
	for (EventList& waitLoading : waitTransferQue) {
		waitLoading.data->postprocess();
	}
}

void BackgroundLoader::transfer_all() {
	for (EventList& waitLoading : waitTransferQue) {
		waitLoading.data->transfer();
	}
	// 転送完了したので削除
	waitTransferQue.clear();
}
