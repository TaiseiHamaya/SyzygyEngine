#pragma once

#include <memory>

namespace szg {

class Scene;
class BaseScene;

class BaseSceneFactory {
public:
	BaseSceneFactory() = default;
	virtual ~BaseSceneFactory() = default;

public:
	virtual std::unique_ptr<Scene> create_scene2(i32 next) const = 0;
};


}; // szg
