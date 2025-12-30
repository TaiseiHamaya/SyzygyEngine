#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "Engine/Module/World/Camera/CameraInstance.h"

#include "Engine/Module/World/WorldInstance/WorldInstance.h"

namespace szg {

class EditorSceneView;

class EditorDebugCamera final : public CameraInstance {
public:
	void initialize() override;
	void update();
	void update_affine() override;

public:
	Vector3 view_point() const;
	r32 offset_imm() const;
	Reference<WorldInstance> constraint_mut() noexcept;

private:
	std::unique_ptr<WorldInstance> constraint;
	r32 offset{ -5.0f };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
