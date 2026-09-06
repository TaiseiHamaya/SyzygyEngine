#include "EmitterInstanceLoader.h"

#include <filesystem>

#include "Engine/Application/Logger.h"
#include "Engine/Assets/IAssetBuilder.h"
#include "Engine/Assets/Json/JsonAsset.h"

#define VECTOR3_SERIALIZER
#define VECTOR2_SERIALIZER
#define COLOR_RGBA_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

using namespace szg;

std::optional<EmitterInstanceSettings> EmitterInstanceLoader::Load(const nlohmann::json& json) {
	if (!json.is_object()) {
		return std::nullopt;
	}
	EmitterInstanceSettings loaded{};
	apply_draw_json(json, loaded);
	apply_schedule_json(json, loaded);
	apply_shape_json(json, loaded);
	apply_direction_json(json, loaded);
	apply_initial_json(json, loaded);
	apply_motion_json(json, loaded);
	apply_rotation_json(json, loaded);
	apply_uv_json(json, loaded);
	apply_flipbook_json(json, loaded);
	apply_fade_json(json, loaded);
	return loaded;
}

void EmitterInstanceLoader::apply_draw_json(const nlohmann::json& json, EmitterInstanceSettings& settings_) {
	nlohmann::json drawJson = json.value("Draw", nlohmann::json::object());
	settings_.drawSpec.drawType = static_cast<ParticleDrawType>(drawJson.value("DrawType", static_cast<u32>(settings_.drawSpec.drawType)));
	settings_.drawSpec.layer = drawJson.value("Layer", settings_.drawSpec.layer);
	settings_.drawSpec.blend = static_cast<BlendMode>(drawJson.value("Blend", static_cast<u32>(settings_.drawSpec.blend)));
	settings_.drawSpec.textureName = drawJson.value("Texture", settings_.drawSpec.textureName);
	settings_.drawSpec.meshName = drawJson.value("Mesh", settings_.drawSpec.meshName);
	if (drawJson.contains("Pivot")) {
		drawJson["Pivot"].get_to(settings_.drawSpec.pivot);
	}
	settings_.overflowPolicy = static_cast<ParticleOverflowPolicy>(drawJson.value("OverflowPolicy", static_cast<u32>(settings_.overflowPolicy)));
	settings_.capacity = drawJson.value("Capacity", settings_.capacity);
	settings_.followParent = drawJson.value("FollowParent", settings_.followParent);
}

void EmitterInstanceLoader::apply_schedule_json(const nlohmann::json& json, EmitterInstanceSettings& settings_) {
	nlohmann::json scheduleJson = json.value("Schedule", nlohmann::json::object());
	settings_.schedule.delay = scheduleJson.value("Delay", settings_.schedule.delay);
	settings_.schedule.interval = scheduleJson.value("Interval", settings_.schedule.interval);
	settings_.schedule.count = scheduleJson.value("Count", settings_.schedule.count);
	settings_.schedule.cycles = scheduleJson.value("Cycles", settings_.schedule.cycles);
	settings_.schedule.infinite = scheduleJson.value("Infinite", settings_.schedule.infinite);
}

void EmitterInstanceLoader::apply_shape_json(const nlohmann::json& json, EmitterInstanceSettings& settings_) {
	nlohmann::json shapeJson = json.value("EmissionShape", nlohmann::json::object());
	settings_.emissionShape.shape = static_cast<ParticleEmissionShapeKind>(shapeJson.value("Shape", static_cast<u32>(settings_.emissionShape.shape)));
	settings_.emissionShape.sphereRadius = shapeJson.value("SphereRadius", settings_.emissionShape.sphereRadius);
	settings_.emissionShape.coneRadius = shapeJson.value("ConeRadius", settings_.emissionShape.coneRadius);
	if (shapeJson.contains("ConeDirection")) {
		shapeJson["ConeDirection"].get_to(settings_.emissionShape.coneDirection);
	}
	settings_.emissionShape.coneAngle = shapeJson.value("ConeAngle", settings_.emissionShape.coneAngle);
	if (shapeJson.contains("BoxSize")) {
		shapeJson["BoxSize"].get_to(settings_.emissionShape.boxSize);
	}
	if (shapeJson.contains("BoxOffset")) {
		shapeJson["BoxOffset"].get_to(settings_.emissionShape.boxOffset);
	}
}

void EmitterInstanceLoader::apply_direction_json(const nlohmann::json& json, EmitterInstanceSettings& settings_) {
	nlohmann::json directionJson = json.value("EmissionDirection", nlohmann::json::object());
	settings_.emissionDirection.directionMode = static_cast<ParticleDirectionMode>(directionJson.value("DirectionMode", static_cast<u32>(settings_.emissionDirection.directionMode)));
	if (directionJson.contains("ConstantDirection")) {
		directionJson["ConstantDirection"].get_to(settings_.emissionDirection.constantDirection);
	}
	if (directionJson.contains("AngleBaseDirection")) {
		directionJson["AngleBaseDirection"].get_to(settings_.emissionDirection.angleBaseDirection);
	}
	settings_.emissionDirection.angleRange = directionJson.value("AngleRange", settings_.emissionDirection.angleRange);
}

void EmitterInstanceLoader::apply_initial_json(const nlohmann::json& json, EmitterInstanceSettings& settings_) {
	nlohmann::json initialJson = json.value("Initial", nlohmann::json::object());
	settings_.initial.lifetimeMin = initialJson.value("LifetimeMin", settings_.initial.lifetimeMin);
	settings_.initial.lifetimeMax = initialJson.value("LifetimeMax", settings_.initial.lifetimeMax);
	settings_.initial.speedMin = initialJson.value("SpeedMin", settings_.initial.speedMin);
	settings_.initial.speedMax = initialJson.value("SpeedMax", settings_.initial.speedMax);
	if (initialJson.contains("SizeMin")) {
		initialJson["SizeMin"].get_to(settings_.initial.sizeMin);
	}
	if (initialJson.contains("SizeMax")) {
		initialJson["SizeMax"].get_to(settings_.initial.sizeMax);
	}
	if (initialJson.contains("ScaleEndMin")) {
		initialJson["ScaleEndMin"].get_to(settings_.initial.scaleEndMin);
	}
	if (initialJson.contains("ScaleEndMax")) {
		initialJson["ScaleEndMax"].get_to(settings_.initial.scaleEndMax);
	}
	if (initialJson.contains("ColorMin")) {
		initialJson["ColorMin"].get_to(settings_.initial.colorMin);
	}
	if (initialJson.contains("ColorMax")) {
		initialJson["ColorMax"].get_to(settings_.initial.colorMax);
	}
	if (initialJson.contains("ColorEndMin")) {
		initialJson["ColorEndMin"].get_to(settings_.initial.colorEndMin);
	}
	if (initialJson.contains("ColorEndMax")) {
		initialJson["ColorEndMax"].get_to(settings_.initial.colorEndMax);
	}
	settings_.initial.easing = static_cast<ParticleEasingMode>(initialJson.value("Easing", static_cast<u32>(settings_.initial.easing)));
	if (initialJson.contains("AccelerationMin")) {
		initialJson["AccelerationMin"].get_to(settings_.initial.accelerationMin);
	}
	if (initialJson.contains("AccelerationMax")) {
		initialJson["AccelerationMax"].get_to(settings_.initial.accelerationMax);
	}
}

void EmitterInstanceLoader::apply_motion_json(const nlohmann::json& json, EmitterInstanceSettings& settings_) {
	nlohmann::json motionJson = json.value("Motion", nlohmann::json::object());
	settings_.motion.useDragGravity = motionJson.value("UseDragGravity", settings_.motion.useDragGravity);
	settings_.motion.drag = motionJson.value("Drag", settings_.motion.drag);
	if (motionJson.contains("Gravity")) {
		motionJson["Gravity"].get_to(settings_.motion.gravity);
	}
}

void EmitterInstanceLoader::apply_rotation_json(const nlohmann::json& json, EmitterInstanceSettings& settings_) {
	nlohmann::json rotationJson = json.value("Rotation", nlohmann::json::object());
	settings_.rotation.rotationKind = rotationJson.value("RotationKind", settings_.rotation.rotationKind);
	if (rotationJson.contains("ConstantRotation")) {
		rotationJson["ConstantRotation"].get_to(settings_.rotation.constantRotation);
	}
	settings_.rotation.angleMin = rotationJson.value("AngleMin", settings_.rotation.angleMin);
	settings_.rotation.angleMax = rotationJson.value("AngleMax", settings_.rotation.angleMax);
	settings_.rotation.angularVelocityMin = rotationJson.value("AngularVelocityMin", settings_.rotation.angularVelocityMin);
	settings_.rotation.angularVelocityMax = rotationJson.value("AngularVelocityMax", settings_.rotation.angularVelocityMax);
	settings_.rotation.randomAngularVelocityMin = rotationJson.value("RandomAngularVelocityMin", settings_.rotation.randomAngularVelocityMin);
	settings_.rotation.randomAngularVelocityMax = rotationJson.value("RandomAngularVelocityMax", settings_.rotation.randomAngularVelocityMax);
	if (rotationJson.contains("LookTarget")) {
		rotationJson["LookTarget"].get_to(settings_.rotation.lookTarget);
	}
	settings_.rotation.lookAtCameraId = rotationJson.value("LookAtCameraId", settings_.rotation.lookAtCameraId);
	if (rotationJson.contains("AxisBillboardAxis")) {
		rotationJson["AxisBillboardAxis"].get_to(settings_.rotation.axisBillboardAxis);
	}
	settings_.rotation.axisBillboardAngle = rotationJson.value("AxisBillboardAngle", settings_.rotation.axisBillboardAngle);
}

void EmitterInstanceLoader::apply_uv_json(const nlohmann::json& json, EmitterInstanceSettings& settings_) {
	nlohmann::json uvJson = json.value("UV", nlohmann::json::object());
	settings_.uv.useUVScroll = uvJson.value("UseUVScroll", settings_.uv.useUVScroll);
	if (uvJson.contains("UVSpeedMin")) {
		uvJson["UVSpeedMin"].get_to(settings_.uv.uvSpeedMin);
	}
	if (uvJson.contains("UVSpeedMax")) {
		uvJson["UVSpeedMax"].get_to(settings_.uv.uvSpeedMax);
	}
}

void EmitterInstanceLoader::apply_flipbook_json(const nlohmann::json& json, EmitterInstanceSettings& settings_) {
	nlohmann::json flipbookJson = json.value("Flipbook", nlohmann::json::object());
	settings_.flipbook.useFlipbook = flipbookJson.value("UseFlipbook", settings_.flipbook.useFlipbook);
	settings_.flipbook.flipColumns = flipbookJson.value("FlipColumns", settings_.flipbook.flipColumns);
	settings_.flipbook.flipRows = flipbookJson.value("FlipRows", settings_.flipbook.flipRows);
	settings_.flipbook.flipFrameTime = flipbookJson.value("FlipFrameTime", settings_.flipbook.flipFrameTime);
	settings_.flipbook.flipLoop = flipbookJson.value("FlipLoop", settings_.flipbook.flipLoop);
}

void EmitterInstanceLoader::apply_fade_json(const nlohmann::json& json, EmitterInstanceSettings& settings_) {
	nlohmann::json fadeJson = json.value("Fade", nlohmann::json::object());
	settings_.fade.useAlphaFade = fadeJson.value("UseAlphaFade", settings_.fade.useAlphaFade);
	settings_.fade.fadeIn = fadeJson.value("FadeIn", settings_.fade.fadeIn);
	settings_.fade.fadeOut = fadeJson.value("FadeOut", settings_.fade.fadeOut);
}

void EmitterInstanceLoader::Save(const EmitterInstanceSettings& settings_, nlohmann::json& json) {
	write_draw_json(json, settings_);
	write_schedule_json(json, settings_);
	write_shape_json(json, settings_);
	write_direction_json(json, settings_);
	write_initial_json(json, settings_);
	write_motion_json(json, settings_);
	write_rotation_json(json, settings_);
	write_uv_json(json, settings_);
	write_flipbook_json(json, settings_);
	write_fade_json(json, settings_);
}

void EmitterInstanceLoader::write_draw_json(nlohmann::json& json, const EmitterInstanceSettings& settings_) {
	nlohmann::json drawJson;
	drawJson["DrawType"] = static_cast<u32>(settings_.drawSpec.drawType);
	drawJson["Layer"] = settings_.drawSpec.layer;
	drawJson["Blend"] = static_cast<u32>(settings_.drawSpec.blend);
	drawJson["Texture"] = settings_.drawSpec.textureName;
	drawJson["Mesh"] = settings_.drawSpec.meshName;
	drawJson["Pivot"] = settings_.drawSpec.pivot;
	drawJson["OverflowPolicy"] = static_cast<u32>(settings_.overflowPolicy);
	drawJson["Capacity"] = settings_.capacity;
	drawJson["FollowParent"] = settings_.followParent;
	json["Draw"] = drawJson;
}

void EmitterInstanceLoader::write_schedule_json(nlohmann::json& json, const EmitterInstanceSettings& settings_) {
	nlohmann::json scheduleJson;
	scheduleJson["Delay"] = settings_.schedule.delay;
	scheduleJson["Interval"] = settings_.schedule.interval;
	scheduleJson["Count"] = settings_.schedule.count;
	scheduleJson["Cycles"] = settings_.schedule.cycles;
	scheduleJson["Infinite"] = settings_.schedule.infinite;
	json["Schedule"] = scheduleJson;
}

void EmitterInstanceLoader::write_shape_json(nlohmann::json& json, const EmitterInstanceSettings& settings_) {
	nlohmann::json shapeJson;
	shapeJson["Shape"] = static_cast<u32>(settings_.emissionShape.shape);
	shapeJson["SphereRadius"] = settings_.emissionShape.sphereRadius;
	shapeJson["ConeRadius"] = settings_.emissionShape.coneRadius;
	shapeJson["ConeDirection"] = settings_.emissionShape.coneDirection;
	shapeJson["ConeAngle"] = settings_.emissionShape.coneAngle;
	shapeJson["BoxSize"] = settings_.emissionShape.boxSize;
	shapeJson["BoxOffset"] = settings_.emissionShape.boxOffset;
	json["EmissionShape"] = shapeJson;
}

void EmitterInstanceLoader::write_direction_json(nlohmann::json& json, const EmitterInstanceSettings& settings_) {
	nlohmann::json directionJson;
	directionJson["DirectionMode"] = static_cast<u32>(settings_.emissionDirection.directionMode);
	directionJson["ConstantDirection"] = settings_.emissionDirection.constantDirection;
	directionJson["AngleBaseDirection"] = settings_.emissionDirection.angleBaseDirection;
	directionJson["AngleRange"] = settings_.emissionDirection.angleRange;
	json["EmissionDirection"] = directionJson;
}

void EmitterInstanceLoader::write_initial_json(nlohmann::json& json, const EmitterInstanceSettings& settings_) {
	nlohmann::json initialJson;
	initialJson["LifetimeMin"] = settings_.initial.lifetimeMin;
	initialJson["LifetimeMax"] = settings_.initial.lifetimeMax;
	initialJson["SpeedMin"] = settings_.initial.speedMin;
	initialJson["SpeedMax"] = settings_.initial.speedMax;
	initialJson["SizeMin"] = settings_.initial.sizeMin;
	initialJson["SizeMax"] = settings_.initial.sizeMax;
	initialJson["ScaleEndMin"] = settings_.initial.scaleEndMin;
	initialJson["ScaleEndMax"] = settings_.initial.scaleEndMax;
	initialJson["ColorMin"] = settings_.initial.colorMin;
	initialJson["ColorMax"] = settings_.initial.colorMax;
	initialJson["ColorEndMin"] = settings_.initial.colorEndMin;
	initialJson["ColorEndMax"] = settings_.initial.colorEndMax;
	initialJson["Easing"] = static_cast<u32>(settings_.initial.easing);
	initialJson["AccelerationMin"] = settings_.initial.accelerationMin;
	initialJson["AccelerationMax"] = settings_.initial.accelerationMax;
	json["Initial"] = initialJson;
}

void EmitterInstanceLoader::write_motion_json(nlohmann::json& json, const EmitterInstanceSettings& settings_) {
	nlohmann::json motionJson;
	motionJson["UseDragGravity"] = settings_.motion.useDragGravity;
	motionJson["Drag"] = settings_.motion.drag;
	motionJson["Gravity"] = settings_.motion.gravity;
	json["Motion"] = motionJson;
}

void EmitterInstanceLoader::write_rotation_json(nlohmann::json& json, const EmitterInstanceSettings& settings_) {
	nlohmann::json rotationJson;
	rotationJson["RotationKind"] = static_cast<u32>(settings_.rotation.rotationKind);
	rotationJson["ConstantRotation"] = settings_.rotation.constantRotation;
	rotationJson["AngleMin"] = settings_.rotation.angleMin;
	rotationJson["AngleMax"] = settings_.rotation.angleMax;
	rotationJson["AngularVelocityMin"] = settings_.rotation.angularVelocityMin;
	rotationJson["AngularVelocityMax"] = settings_.rotation.angularVelocityMax;
	rotationJson["RandomAngularVelocityMin"] = settings_.rotation.randomAngularVelocityMin;
	rotationJson["RandomAngularVelocityMax"] = settings_.rotation.randomAngularVelocityMax;
	rotationJson["LookTarget"] = settings_.rotation.lookTarget;
	rotationJson["LookAtCameraId"] = settings_.rotation.lookAtCameraId;
	rotationJson["AxisBillboardAxis"] = settings_.rotation.axisBillboardAxis;
	rotationJson["AxisBillboardAngle"] = settings_.rotation.axisBillboardAngle;
	json["Rotation"] = rotationJson;
}

void EmitterInstanceLoader::write_uv_json(nlohmann::json& json, const EmitterInstanceSettings& settings_) {
	nlohmann::json uvJson;
	uvJson["UseUVScroll"] = settings_.uv.useUVScroll;
	uvJson["UVSpeedMin"] = settings_.uv.uvSpeedMin;
	uvJson["UVSpeedMax"] = settings_.uv.uvSpeedMax;
	json["UV"] = uvJson;
}

void EmitterInstanceLoader::write_flipbook_json(nlohmann::json& json, const EmitterInstanceSettings& settings_) {
	nlohmann::json flipbookJson;
	flipbookJson["UseFlipbook"] = settings_.flipbook.useFlipbook;
	flipbookJson["FlipColumns"] = settings_.flipbook.flipColumns;
	flipbookJson["FlipRows"] = settings_.flipbook.flipRows;
	flipbookJson["FlipFrameTime"] = settings_.flipbook.flipFrameTime;
	flipbookJson["FlipLoop"] = settings_.flipbook.flipLoop;
	json["Flipbook"] = flipbookJson;
}

void EmitterInstanceLoader::write_fade_json(nlohmann::json& json, const EmitterInstanceSettings& settings_) {
	nlohmann::json fadeJson;
	fadeJson["UseAlphaFade"] = settings_.fade.useAlphaFade;
	fadeJson["FadeIn"] = settings_.fade.fadeIn;
	fadeJson["FadeOut"] = settings_.fade.fadeOut;
	json["Fade"] = fadeJson;
}