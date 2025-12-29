#include "SceneManager2.h"

using namespace szg;

#include "Engine/Application/Logger.h"
#include "Engine/Application/WinApp.h"
#include "Engine/Assets/BackgroundLoader/BackgroundLoader.h"
#include "Engine/Runtime/Scene/BaseSceneFactory.h"

void SceneManager2::Initialize() {
	SceneManager2& instance = GetInstance();
	szgCriticalIf(!instance.sceneStack.empty(), "Scene manager is already initialized.");

	// 最初にnullptrをemplace_backする
	instance.sceneStack.emplace_back(nullptr);
	instance.sceneChangeTempData.onEnd = { &SceneManager2::OnNextScene, 0, true };
}

void SceneManager2::Setup(std::unique_ptr<BaseSceneFactory> factory_) {
	SceneManager2& instance = GetInstance();
	instance.factory = std::move(factory_);

	auto scene = instance.factory->initialize_scene2();
	if (!scene) {
		szgCritical("The created initial scene was nullptr.");
		return;
	}

	scene->load_assets();
	scene->custom_load_asset();

	instance.sceneStack.emplace_back(std::move(scene));

	BackgroundLoader::WaitEndExecute();

	auto& currentScene = instance.sceneStack.back();
	currentScene->initialize();
	currentScene->setup();
	currentScene->custom_setup();
}

void SceneManager2::Finalize() noexcept {
	SceneManager2& instance = GetInstance();
	for (auto& scene : instance.sceneStack) {
		if (!scene) {
			continue;
		}
		scene->finalize();
	}
	instance.sceneStack.clear();
	instance.factory.reset();
}

void SceneManager2::BeginFrame() {
	auto& instance = GetInstance();
	if (!instance.sceneChangeTempData.onEnd.is_finished()) {
		instance.sceneChangeTempData.onEnd.update();
	}

	auto& currentScene = instance.sceneStack.back();
	currentScene->begin_frame();
}

void SceneManager2::Update() {
	auto& instance = GetInstance();
	auto& currentScene = instance.sceneStack.back();

	currentScene->update();
}

void SceneManager2::PreDraw() {
	auto& instance = GetInstance();
	auto& currentScene = instance.sceneStack.back();

	currentScene->pre_draw();
}

void SceneManager2::Draw() {
	auto& instance = GetInstance();
	auto& currentScene = instance.sceneStack.back();

	currentScene->draw();
}

void SceneManager2::EndFrame() {
	auto& instance = GetInstance();
	auto& currentScene = instance.sceneStack.back();
	currentScene->end_frame();
}

bool SceneManager2::IsEndProgram() noexcept {
	return GetInstance().sceneStack.back() == nullptr;
}

void SceneManager2::SceneChange(u32 nextSceneIndex, r32 interval, bool isStackScene, bool isStopLoad) {
	SceneManager2& instance = GetInstance();

	if (!instance.sceneChangeTempData.onEnd.is_finished()) {
		szgWarning("Scene is changing now. Cannot change scene.");
		return;
	}

	// 生成
	auto nextScene = instance.factory->create_scene2(nextSceneIndex);

	// アセットロード
	nextScene->load_assets();
	nextScene->custom_load_asset();

	// シーンスタック
	std::swap(instance.sceneStack.back(), nextScene);
	instance.sceneStack.emplace_back(std::move(nextScene));

	// 遷移時に必要な情報を保存
	if (isStackScene) {
		instance.sceneChangeTempData.type = SceneChangeType::STACK;
	}
	else {
		instance.sceneChangeTempData.type = SceneChangeType::DEFAULT;
	}
	instance.sceneChangeTempData.isStopLoad = isStopLoad;
	instance.sceneChangeTempData.onEnd.restart(interval);
}

void SceneManager2::PopScene(r32 interval, size_t numPopScenes) {
	SceneManager2& instance = GetInstance();

	if (!instance.sceneChangeTempData.onEnd.is_finished()) {
		szgWarning("Scene is changing now. Cannot change scene.");
		return;
	}

	size_t sceneSize = instance.sceneStack.size();
	if (numPopScenes >= sceneSize) {
		// シーンを全てPopしないように調整
		numPopScenes = sceneSize - 1;
		szgWarning("NumPopScenes is bigger than current scene stack size. Adjusted to \'{}\'.", numPopScenes);
	}

	size_t targetIndex = sceneSize - numPopScenes - 1;

	std::swap(instance.sceneStack.back(), instance.sceneStack[targetIndex]);
	for (size_t i = 0; i + 1 < numPopScenes; i++) {
		instance.sceneStack.back()->finalize();
		instance.sceneStack.pop_back();
	}

	// 遷移時の情報
	instance.sceneChangeTempData.type = SceneChangeType::POP;
	instance.sceneChangeTempData.isStopLoad = false;
	instance.sceneChangeTempData.onEnd.restart(interval);
}

Reference<Scene> SceneManager2::GetCurrentScene() {
	return GetInstance().sceneStack.back();
}

void SceneManager2::OnNextScene() {
	SceneManager2& instance = GetInstance();

	// シーンをスタックする場合
	if (instance.sceneChangeTempData.type != SceneChangeType::DEFAULT) {
		std::swap(instance.sceneStack.back(), instance.sceneStack[instance.sceneStack.size() - 2]);
	}
	// 入れ替える場合
	else {
		instance.sceneStack.back()->finalize();
		instance.sceneStack.pop_back();
	}

	if (instance.sceneChangeTempData.isStopLoad) {
		// ロード完了待ち
		BackgroundLoader::WaitEndExecute();
	}

	// シーンの初期化
	auto& newScene = instance.sceneStack.back();
	if (instance.sceneChangeTempData.type != SceneChangeType::POP) {
		newScene->initialize();
		newScene->setup();
		newScene->custom_setup();
	}
}
