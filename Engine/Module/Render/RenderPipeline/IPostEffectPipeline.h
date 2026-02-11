#pragma once

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "./BaseRenderPipeline.h"

namespace szg {

class IPostEffectPipeline : public BaseRenderPipeline {
public:
	IPostEffectPipeline() = default;
	virtual ~IPostEffectPipeline() = default;

	SZG_CLASS_MOVE_ONLY(IPostEffectPipeline)

public:
	virtual void execute_effect_command() = 0;
};

}; // szg
