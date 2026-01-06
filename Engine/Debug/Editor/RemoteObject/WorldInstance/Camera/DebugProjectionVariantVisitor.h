#pragma once

#include "./RemoteCamera3dInstance.h"
#include "Engine/Module/World/Camera/ProjectionAdapter/CameraProjectionTypeEnum.h"

namespace szg {

class DebugProjectionVariantVisitor {
public:
	void operator()(szg::RemoteCamera3dInstance::PerspectiveParameters& param) {
		param.fovY.show_gui();
		param.aspectRatio.show_gui();
		param.nearClip.show_gui();
		param.farClip.show_gui();
	}

	void operator()(szg::RemoteCamera3dInstance::OrthroParameters& param) {
		param.left.show_gui();
		param.right.show_gui();
		param.bottom.show_gui();
		param.top.show_gui();
		param.nearClip.show_gui();
		param.farClip.show_gui();
	}
};

class DebugProjectionVariantSaver {
public:
	nlohmann::json operator()(const szg::RemoteCamera3dInstance::PerspectiveParameters& param) {
		nlohmann::json result;
		result["Type"] = szg::CameraProjectionTypeEnum::Perspective;
		result.update(param.fovY);
		result.update(param.aspectRatio);
		result.update(param.nearClip);
		result.update(param.farClip);
		return result;
	}
	nlohmann::json operator()(const szg::RemoteCamera3dInstance::OrthroParameters& param) {
		nlohmann::json result;
		result["Type"] = szg::CameraProjectionTypeEnum::Orthographic;
		result.update(param.left);
		result.update(param.right);
		result.update(param.bottom);
		result.update(param.top);
		result.update(param.nearClip);
		result.update(param.farClip);
		return result;
	}
};

}; // szg
