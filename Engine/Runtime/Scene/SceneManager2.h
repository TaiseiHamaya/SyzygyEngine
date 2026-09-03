#pragma once

#include <memory>
#include <vector>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Template/SingletonInterface.h>
#include <Library/Utility/Template/TimedCall.h>

#include "Scene.h"

namespace szg {

class BaseSceneFactory;

#ifdef DEBUG_FEATURES_ENABLE
class TimestampProfiler;
#endif //DEBUG_FEATURES_ENABLE

class SceneManager2 final : public SingletonInterface<SceneManager2> {
	SZG_CLASS_SINGLETON(SceneManager2)

public:
	static void Initialize();
	static void SetupFactory(std::unique_ptr<BaseSceneFactory> factory_);
	static void SetupInitialScene(i32 initialSceneIndex_);
	static void Setup();
	static void Finalize() noexcept;

	static void BeginFrame();
	static void Update();
	static void PreDraw();
	static void Draw();
	static void EndFrame();

public:
	static bool IsEndProgram() noexcept;

	static void SceneChange(u32 nextScene, r32 interval, bool isStackScene = false, bool isStopLoad = true);
	static void PopScene(r32 interval, size_t numPopScenes = 1);

	static void TryChangeScene();

	static void EndSceneChangeIntervalForce();

public:
	static Reference<Scene> GetCurrentScene();

	static Reference<const BaseSceneFactory> SceneFactoryImm() noexcept;

private:
	static void OnNextScene();

#ifdef DEBUG_FEATURES_ENABLE
public:
	static void SetProfiler(Reference<TimestampProfiler> profiler_);
#endif // _DEBUG

private:
	std::unique_ptr<BaseSceneFactory> factory;
	i32 initialSceneIndex{ 0 };

	std::vector<std::unique_ptr<Scene>> sceneStack;

	enum class SceneChangeType {
		NONE,
		DEFAULT,
		STACK,
		POP,
	};

	struct SceneChangeTempData {
		TimedCall<void()> onEnd;
		SceneChangeType type = SceneChangeType::NONE;
		bool isStopLoad;
	} sceneChangeTempData;

#ifdef DEBUG_FEATURES_ENABLE
private:
	Reference<TimestampProfiler> profiler;
#endif // _DEBUG
};

}; // szg
