#ifdef DEBUG_FEATURES_ENABLE

#include "RemoteEmitterInstance.h"

#include <filesystem>

#include <imgui.h>

#include "Engine/Assets/IAssetBuilder.h"
#include "Engine/Assets/Json/JsonAsset.h"
#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"
#include "Engine/Debug/Editor/Window/SceneView/EditorSceneView.h"
#include "Engine/Module/World/Particle/EmitterInstanceLoader.h"

using namespace szg;

namespace {

string_literal DRAW_TYPE_ITEMS[] = { "Billboard", "Mesh" };
string_literal BLEND_ITEMS[] = { "None", "Alpha", "Add", "Subtract", "Multily", "Screen" };
string_literal OVERFLOW_ITEMS[] = { "Discard", "ReuseOldest" };
string_literal SHAPE_ITEMS[] = { "Point", "Sphere", "Cone", "Box" };
string_literal DIRECTION_ITEMS[] = { "Constant", "EmissionShape", "AngleRange" };
string_literal EASING_ITEMS[] = { "Linear", "In", "Out", "InOut" };
string_literal ROTATION_ITEMS[] = { "Billboard", "Constant", "VelocityAlign", "Random", "AxisBillboard" };

}

void RemoteEmitterInstance::draw_combo(EditorValueField<u32>& field, string_literal label, const string_literal items[], u32 itemCount) {
	i32 current = static_cast<i32>(field.value_imm());
	if (current < 0 || static_cast<u32>(current) >= itemCount) {
		current = 0;
	}
	if (ImGui::Combo(label, &current, items, static_cast<i32>(itemCount))) {
		field = static_cast<u32>(current);
	}
}

void RemoteEmitterInstance::setup() {
	debugVisual = std::make_unique<Rect3d>();
	debugVisual->initialize(CVector2::HALF, CVector2::HALF);
	debugVisual->get_material().lightingType = LighingType::None;
	debugVisual->get_material().texture = TextureLibrary::GetTexture("EngineIcon_Emitter.png");

	sceneView->register_rect(query_world(), debugVisual);

	rebuild_preview();
	RemoteInstanceType::setup();
}

void RemoteEmitterInstance::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	RemoteInstanceType::update_preview(world, parentAffine);

	switch (static_cast<ParticleEmissionShapeKind>(emission.shape.value_imm())) {
	case ParticleEmissionShapeKind::Sphere:
	{
		r32 radius = emission.sphereRadius.value_imm();
		Affine primitiveAffine = Affine::FromScale(Vector3{ radius, radius, radius }) * worldAffine;
		sceneView->write_primitive(world, "Sphere", primitiveAffine);
		break;
	}
	case ParticleEmissionShapeKind::Box:
	{
		Affine primitiveAffine = Affine::FromScale(emission.boxSize.value_imm()) * Affine::FromTranslate(emission.boxOffset.value_imm()) * worldAffine;
		sceneView->write_primitive(world, "Box", primitiveAffine);
		break;
	}
	case ParticleEmissionShapeKind::Point:
	case ParticleEmissionShapeKind::Cone:
	default:
		break;
	}

	if (!previewEmitter || !previewPool) {
		rebuild_preview();
	}
	if (previewEmitter && previewPool && isPreviewPlaying.value_imm()) {
		previewEmitter->transform_mut() = transform.value_mut();
		previewEmitter->update_affine();
		previewEmitter->update();
		previewUpdaters.update_pool(previewPool.get());
		previewPool->reclaim();
	}
	if (previewPool) {
		poolView.refresh(previewPool.get());
	}

	if (debugVisual) {
		Reference<const EditorDebugCamera> camera = sceneView->query_debug_camera();
		if (camera) {
			debugVisual->look_at(camera);
		}
		debugVisual->transform_mut().set_translate(worldAffine.get_origin());
		debugVisual->update_affine();
	}
}

void RemoteEmitterInstance::draw_inspector() {
	ImGui::Text("Type : EmitterInstance");

	hierarchyName.show_gui();
	isUseRuntime.show_gui();

	ImGui::Separator();
	transform.show_gui();
	ImGui::Separator();

	inspect_file();

	inspect_draw();
	inspect_schedule();
	inspect_emission();
	inspect_initial();
	inspect_rotation();
	inspect_options();
	inspect_preview();
}

void RemoteEmitterInstance::inspect_draw() {
	if (ImGui::TreeNodeEx("Draw", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllColumns)) {
		draw_combo(draw.drawType, "DrawType", DRAW_TYPE_ITEMS, 2);
		if (draw.drawType.value_imm() == 0) {
			draw.textureName.show_gui();
		}
		else {
			draw.meshName.show_gui();
			draw.textureName.show_gui();
		}
		draw_combo(draw.blend, "Blend", BLEND_ITEMS, 6);
		draw.layer.show_gui();
		draw.pivot.show_gui();
		draw_combo(draw.overflowPolicy, "OverflowPolicy", OVERFLOW_ITEMS, 2);
		draw.capacity.show_gui();
		draw.followParent.show_gui();
		ImGui::TreePop();
	}
}

void RemoteEmitterInstance::inspect_schedule() {
	if (ImGui::TreeNodeEx("Schedule", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllColumns)) {
		schedule.delay.show_gui();
		schedule.interval.show_gui();
		schedule.count.show_gui();
		schedule.cycles.show_gui();
		schedule.infinite.show_gui();
		ImGui::TreePop();
	}
}

void RemoteEmitterInstance::inspect_emission() {
	if (ImGui::TreeNodeEx("Emission", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllColumns)) {
		draw_combo(emission.shape, "Shape", SHAPE_ITEMS, 4);
		switch (static_cast<ParticleEmissionShapeKind>(emission.shape.value_imm())) {
		case ParticleEmissionShapeKind::Sphere:
			emission.sphereRadius.show_gui();
			break;
		case ParticleEmissionShapeKind::Cone:
			emission.coneRadius.show_gui();
			emission.coneDirection.show_gui();
			emission.coneAngle.show_gui();
			break;
		case ParticleEmissionShapeKind::Box:
			emission.boxSize.show_gui();
			emission.boxOffset.show_gui();
			break;
		case ParticleEmissionShapeKind::Point:
		default:
			break;
		}
		draw_combo(emission.directionMode, "DirectionMode", DIRECTION_ITEMS, 3);
		switch (static_cast<ParticleDirectionMode>(emission.directionMode.value_imm())) {
		case ParticleDirectionMode::Constant:
			emission.constantDirection.show_gui();
			break;
		case ParticleDirectionMode::AngleRange:
			emission.angleBaseDirection.show_gui();
			emission.angleRange.show_gui();
			break;
		case ParticleDirectionMode::EmissionShape:
		default:
			break;
		}
		ImGui::TreePop();
	}
}

void RemoteEmitterInstance::inspect_initial() {
	if (ImGui::TreeNodeEx("Initial", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllColumns)) {
		initial.lifetimeMin.show_gui();
		initial.lifetimeMax.show_gui();
		initial.speedMin.show_gui();
		initial.speedMax.show_gui();
		initial.sizeMin.show_gui();
		initial.sizeMax.show_gui();
		initial.scaleEndMin.show_gui();
		initial.scaleEndMax.show_gui();
		initial.colorMin.show_gui();
		initial.colorMax.show_gui();
		initial.colorEndMin.show_gui();
		initial.colorEndMax.show_gui();
		draw_combo(initial.easing, "Easing", EASING_ITEMS, 4);
		initial.accelerationMin.show_gui();
		initial.accelerationMax.show_gui();
		initial.useDragGravity.show_gui();
		if (initial.useDragGravity.value_imm()) {
			initial.drag.show_gui();
			initial.gravity.show_gui();
		}
		ImGui::TreePop();
	}
}

void RemoteEmitterInstance::inspect_rotation() {
	if (ImGui::TreeNodeEx("Rotation", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllColumns)) {
		draw_combo(rotation.rotationKind, "RotationKind", ROTATION_ITEMS, 5);
		switch (static_cast<ParticleRotationKind>(rotation.rotationKind.value_imm())) {
		case ParticleRotationKind::Billboard:
			rotation.angleMin.show_gui();
			rotation.angleMax.show_gui();
			rotation.angularVelocityMin.show_gui();
			rotation.angularVelocityMax.show_gui();
			rotation.lookTarget.show_gui();
			rotation.lookAtCameraId.show_gui();
			break;
		case ParticleRotationKind::Random:
			rotation.randomAngularVelocityMin.show_gui();
			rotation.randomAngularVelocityMax.show_gui();
			break;
		case ParticleRotationKind::Constant:
			rotation.constantRotation.show_gui();
			break;
		case ParticleRotationKind::AxisBillboard:
			rotation.axisBillboardAxis.show_gui();
			rotation.axisBillboardAngle.show_gui();
			rotation.lookTarget.show_gui();
			rotation.lookAtCameraId.show_gui();
			break;
		case ParticleRotationKind::VelocityAlign:
		default:
			break;
		}
		ImGui::TreePop();
	}
}

void RemoteEmitterInstance::inspect_options() {
	if (ImGui::TreeNodeEx("Options", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllColumns)) {
		options.useUVScroll.show_gui();
		if (options.useUVScroll.value_imm()) {
			options.uvSpeedMin.show_gui();
			options.uvSpeedMax.show_gui();
		}
		options.useFlipbook.show_gui();
		if (options.useFlipbook.value_imm()) {
			options.flipColumns.show_gui();
			options.flipRows.show_gui();
			options.flipFrameTime.show_gui();
			options.flipLoop.show_gui();
		}
		options.useAlphaFade.show_gui();
		if (options.useAlphaFade.value_imm()) {
			options.fadeIn.show_gui();
			options.fadeOut.show_gui();
		}
		ImGui::TreePop();
	}
}

void RemoteEmitterInstance::inspect_file() {
	if (ImGui::TreeNodeEx("File", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllColumns)) {
		if (!options.particleFile.value_imm().empty()) {
			std::filesystem::path relativePath = IAssetBuilder::ResolveFilePath("[[game]]/" + options.particleFile.value_imm(), "particle");
			std::error_code errorCode{};
			if (!std::filesystem::exists(relativePath, errorCode)) {
				ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "File not found.");
			}
		}
		if (options.particleFile.show_gui().any()) {
			import_particles();
		}
		if (ImGui::Button("Import")) {
			import_particles();
		}

		ImGui::SameLine();
		if (ImGui::Button("Detach")) {
			EditorValueChangeCommandHandler::GenCommandInstant<std::string>(options.particleFile.value_mut(), "");
		}
		ImGui::TreePop();
	}
}

void RemoteEmitterInstance::inspect_preview() {
	if (ImGui::TreeNodeEx("Preview", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllColumns)) {
		isPreviewPlaying.show_gui();
		if (ImGui::Button("Burst")) {
			if (previewEmitter && previewPool) {
				previewEmitter->transform_mut() = transform.value_mut();
				previewEmitter->update_affine();
				for (u32 i = 0; i < schedule.count.value_imm(); ++i) {
					previewUpdaters.update_pool(previewPool.get());
				}
				poolView.refresh(previewPool.get());
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear")) {
			if (previewPool) {
				previewPool->clear();
				poolView.refresh(previewPool.get());
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Rebuild")) {
			rebuild_preview();
		}
		poolView.draw_inspector();
		ImGui::TreePop();
	}
}

nlohmann::json RemoteEmitterInstance::serialize() const {
	nlohmann::json json;

	json.update(hierarchyName);
	json.update(isUseRuntime);
	json.update(transform);
	json["Type"] = instance_type();

	std::string particleFile = options.particleFile.value_imm().empty() ? hierarchyName.value_imm() + ".particle" : options.particleFile.value_imm();
	std::string exportFile = std::format("[[game]]/{}", particleFile);
	json["ParticleFile"] = particleFile;

	nlohmann::json exported;
	EmitterInstanceLoader::Save(build_settings(), exported);
	JsonAsset asset{ exportFile };
	asset.get() = exported;
	asset.save();

	return json;
}

void RemoteEmitterInstance::on_spawn() {
	draw.textureName.on_activated();
	draw.meshName.on_activated();
	auto world = query_world();
	auto result = sceneView->get_layer(world);
	debugVisual->set_layer(result.value_or(-1));
	RemoteInstanceType::on_spawn();
}

void RemoteEmitterInstance::on_destroy() {
	draw.textureName.on_deactivated();
	draw.meshName.on_deactivated();
	RemoteInstanceType::on_destroy();
}

EmitterInstanceSettings RemoteEmitterInstance::build_settings() const {
	EmitterInstanceSettings built{};
	built.drawSpec.drawType = static_cast<ParticleDrawType>(draw.drawType.value_imm());
	built.drawSpec.textureName = draw.textureName.value_imm();
	built.drawSpec.meshName = draw.meshName.value_imm();
	built.drawSpec.blend = static_cast<BlendMode>(draw.blend.value_imm());
	built.drawSpec.layer = draw.layer.value_imm();
	built.drawSpec.pivot = draw.pivot.value_imm();

	built.overflowPolicy = static_cast<ParticleOverflowPolicy>(draw.overflowPolicy.value_imm());
	built.capacity = draw.capacity.value_imm();
	built.followParent = draw.followParent.value_imm();

	built.schedule.delay = schedule.delay.value_imm();
	built.schedule.interval = schedule.interval.value_imm();
	built.schedule.count = schedule.count.value_imm();
	built.schedule.cycles = schedule.cycles.value_imm();
	built.schedule.infinite = schedule.infinite.value_imm();

	built.emissionShape.shape = static_cast<ParticleEmissionShapeKind>(emission.shape.value_imm());
	built.emissionShape.sphereRadius = emission.sphereRadius.value_imm();
	built.emissionShape.coneRadius = emission.coneRadius.value_imm();
	built.emissionShape.coneDirection = emission.coneDirection.value_imm();
	built.emissionShape.coneAngle = emission.coneAngle.value_imm();
	built.emissionShape.boxSize = emission.boxSize.value_imm();
	built.emissionShape.boxOffset = emission.boxOffset.value_imm();
	built.emissionDirection.directionMode = static_cast<ParticleDirectionMode>(emission.directionMode.value_imm());
	built.emissionDirection.constantDirection = emission.constantDirection.value_imm();
	built.emissionDirection.angleBaseDirection = emission.angleBaseDirection.value_imm();
	built.emissionDirection.angleRange = emission.angleRange.value_imm();

	built.initial.lifetimeMin = initial.lifetimeMin.value_imm();
	built.initial.lifetimeMax = initial.lifetimeMax.value_imm();
	built.initial.speedMin = initial.speedMin.value_imm();
	built.initial.speedMax = initial.speedMax.value_imm();
	built.initial.sizeMin = initial.sizeMin.value_imm();
	built.initial.sizeMax = initial.sizeMax.value_imm();
	built.initial.scaleEndMin = initial.scaleEndMin.value_imm();
	built.initial.scaleEndMax = initial.scaleEndMax.value_imm();
	built.initial.colorMin = initial.colorMin.value_imm();
	built.initial.colorMax = initial.colorMax.value_imm();
	built.initial.colorEndMin = initial.colorEndMin.value_imm();
	built.initial.colorEndMax = initial.colorEndMax.value_imm();
	built.initial.easing = static_cast<ParticleEasingMode>(initial.easing.value_imm());
	built.initial.accelerationMin = initial.accelerationMin.value_imm();
	built.initial.accelerationMax = initial.accelerationMax.value_imm();

	built.motion.drag = initial.drag.value_imm();
	built.motion.gravity = initial.gravity.value_imm();

	built.rotation.rotationKind = static_cast<ParticleRotationKind>(rotation.rotationKind.value_imm());
	built.rotation.constantRotation = rotation.constantRotation.value_imm();
	built.rotation.angleMin = rotation.angleMin.value_imm();
	built.rotation.angleMax = rotation.angleMax.value_imm();
	built.rotation.angularVelocityMin = rotation.angularVelocityMin.value_imm();
	built.rotation.angularVelocityMax = rotation.angularVelocityMax.value_imm();
	built.rotation.randomAngularVelocityMin = rotation.randomAngularVelocityMin.value_imm();
	built.rotation.randomAngularVelocityMax = rotation.randomAngularVelocityMax.value_imm();
	built.rotation.lookTarget = rotation.lookTarget.value_imm();
	built.rotation.lookAtCameraId = rotation.lookAtCameraId.value_imm();
	built.rotation.axisBillboardAxis = rotation.axisBillboardAxis.value_imm();
	built.rotation.axisBillboardAngle = rotation.axisBillboardAngle.value_imm();

	built.motion.useDragGravity = initial.useDragGravity.value_imm();

	built.uv.useUVScroll = options.useUVScroll.value_imm();
	built.uv.uvSpeedMin = options.uvSpeedMin.value_imm();
	built.uv.uvSpeedMax = options.uvSpeedMax.value_imm();

	built.flipbook.useFlipbook = options.useFlipbook.value_imm();
	built.flipbook.flipColumns = options.flipColumns.value_imm();
	built.flipbook.flipRows = options.flipRows.value_imm();
	built.flipbook.flipFrameTime = options.flipFrameTime.value_imm();
	built.flipbook.flipLoop = options.flipLoop.value_imm();

	built.fade.useAlphaFade = options.useAlphaFade.value_imm();
	built.fade.fadeIn = options.fadeIn.value_imm();
	built.fade.fadeOut = options.fadeOut.value_imm();
	return built;
}

void RemoteEmitterInstance::apply_settings(const EmitterInstanceSettings& settings_) {
	draw.drawType.set_weak(static_cast<u32>(settings_.drawSpec.drawType));
	draw.textureName.set_weak(settings_.drawSpec.textureName);
	draw.meshName.set_weak(settings_.drawSpec.meshName);
	draw.blend.set_weak(static_cast<u32>(settings_.drawSpec.blend));
	draw.layer.set_weak(settings_.drawSpec.layer);
	draw.pivot.set_weak(settings_.drawSpec.pivot);
	draw.overflowPolicy.set_weak(static_cast<u32>(settings_.overflowPolicy));
	draw.capacity.set_weak(settings_.capacity);
	draw.followParent.set_weak(settings_.followParent);

	schedule.delay.set_weak(settings_.schedule.delay);
	schedule.interval.set_weak(settings_.schedule.interval);
	schedule.count.set_weak(settings_.schedule.count);
	schedule.cycles.set_weak(settings_.schedule.cycles);
	schedule.infinite.set_weak(settings_.schedule.infinite);

	emission.shape.set_weak(static_cast<u32>(settings_.emissionShape.shape));
	emission.sphereRadius.set_weak(settings_.emissionShape.sphereRadius);
	emission.coneRadius.set_weak(settings_.emissionShape.coneRadius);
	emission.coneDirection.set_weak(settings_.emissionShape.coneDirection);
	emission.coneAngle.set_weak(settings_.emissionShape.coneAngle);
	emission.boxSize.set_weak(settings_.emissionShape.boxSize);
	emission.boxOffset.set_weak(settings_.emissionShape.boxOffset);
	emission.directionMode.set_weak(static_cast<u32>(settings_.emissionDirection.directionMode));
	emission.constantDirection.set_weak(settings_.emissionDirection.constantDirection);
	emission.angleBaseDirection.set_weak(settings_.emissionDirection.angleBaseDirection);
	emission.angleRange.set_weak(settings_.emissionDirection.angleRange);

	initial.lifetimeMin.set_weak(settings_.initial.lifetimeMin);
	initial.lifetimeMax.set_weak(settings_.initial.lifetimeMax);
	initial.speedMin.set_weak(settings_.initial.speedMin);
	initial.speedMax.set_weak(settings_.initial.speedMax);
	initial.sizeMin.set_weak(settings_.initial.sizeMin);
	initial.sizeMax.set_weak(settings_.initial.sizeMax);
	initial.scaleEndMin.set_weak(settings_.initial.scaleEndMin);
	initial.scaleEndMax.set_weak(settings_.initial.scaleEndMax);
	initial.colorMin.set_weak(settings_.initial.colorMin);
	initial.colorMax.set_weak(settings_.initial.colorMax);
	initial.colorEndMin.set_weak(settings_.initial.colorEndMin);
	initial.colorEndMax.set_weak(settings_.initial.colorEndMax);
	initial.easing.set_weak(static_cast<u32>(settings_.initial.easing));
	initial.accelerationMin.set_weak(settings_.initial.accelerationMin);
	initial.accelerationMax.set_weak(settings_.initial.accelerationMax);
	initial.drag.set_weak(settings_.motion.drag);
	initial.gravity.set_weak(settings_.motion.gravity);

	rotation.rotationKind.set_weak(static_cast<u32>(settings_.rotation.rotationKind));
	rotation.constantRotation.set_weak(settings_.rotation.constantRotation);
	rotation.angleMin.set_weak(settings_.rotation.angleMin);
	rotation.angleMax.set_weak(settings_.rotation.angleMax);
	rotation.angularVelocityMin.set_weak(settings_.rotation.angularVelocityMin);
	rotation.angularVelocityMax.set_weak(settings_.rotation.angularVelocityMax);
	rotation.randomAngularVelocityMin.set_weak(settings_.rotation.randomAngularVelocityMin);
	rotation.randomAngularVelocityMax.set_weak(settings_.rotation.randomAngularVelocityMax);
	rotation.lookTarget.set_weak(settings_.rotation.lookTarget);
	rotation.lookAtCameraId.set_weak(settings_.rotation.lookAtCameraId);
	rotation.axisBillboardAxis.set_weak(settings_.rotation.axisBillboardAxis);
	rotation.axisBillboardAngle.set_weak(settings_.rotation.axisBillboardAngle);
	initial.useDragGravity.set_weak(settings_.motion.useDragGravity);

	options.useUVScroll.set_weak(settings_.uv.useUVScroll);
	options.uvSpeedMin.set_weak(settings_.uv.uvSpeedMin);
	options.uvSpeedMax.set_weak(settings_.uv.uvSpeedMax);
	options.useFlipbook.set_weak(settings_.flipbook.useFlipbook);
	options.flipColumns.set_weak(settings_.flipbook.flipColumns);
	options.flipRows.set_weak(settings_.flipbook.flipRows);
	options.flipFrameTime.set_weak(settings_.flipbook.flipFrameTime);
	options.flipLoop.set_weak(settings_.flipbook.flipLoop);
	options.useAlphaFade.set_weak(settings_.fade.useAlphaFade);
	options.fadeIn.set_weak(settings_.fade.fadeIn);
	options.fadeOut.set_weak(settings_.fade.fadeOut);
}

void RemoteEmitterInstance::import_particles() {
	JsonAsset asset{ "[[game]]/" + options.particleFile.value_imm() };
	std::optional<EmitterInstanceSettings> loaded = EmitterInstanceLoader::Load(asset.cget());
	if (!loaded) {
		szgWarning("Particle file is invalid. Path-\'{}\'", options.particleFile.value_imm());
		return;
	}
	apply_settings(*loaded);
	rebuild_preview();
}

void RemoteEmitterInstance::rebuild_preview() {
	EmitterInstanceSettings built = build_settings();
	previewUpdaters.clear_all();
	previewEmitter = std::make_unique<EmitterInstance>();
	previewEmitter->setup_settings(built);
	previewPool = std::make_unique<ParticlePool>();
	previewPool->setup(previewUpdaters, previewEmitter.get(), built.capacity == 0 ? 1 : built.capacity, built.overflowPolicy);
	previewPool->setup_draw_spec(built.drawSpec);
	previewPool->setup_updaters(EmitterInstance::BuildUpdaterMask(built), built.rotation.rotationKind);
	previewEmitter->setup_pool(previewPool.get());
	poolView.refresh(previewPool.get());
}

#endif // DEBUG_FEATURES_ENABLE
