#include "ParticleEmitterInstance.h"

using namespace szg;

#include <Library/Math/Definition.h>
#include <Library/Utility/Tools/RandomEngine.h>

#include "Engine/Module/Manager/World/WorldRoot.h"
#include "./DrawSystem/ParticleDrawSystemMesh.h"
#include "./DrawSystem/ParticleDrawSystemRect.h"
#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"
#include "Engine/Assets/Texture/TextureAsset.h"
#include "Engine/Assets/Texture/TextureLibrary.h"

ParticleEmitterInstance::ParticleEmitterInstance(std::filesystem::path jsonFile, u32 MaxParticle) :
	WorldInstance(),
	numMaxParticle(MaxParticle),
	jsonResource("Particle" / jsonFile) {
	timer.set(0);
	drawType = static_cast<ParticleDrawType>(jsonResource.try_emplace<i32>("DrawType"));
	switch (drawType) {
	case ParticleDrawType::Mesh:
		useResourceName = jsonResource.try_emplace<std::string>("useResourceName");
		break;
	case ParticleDrawType::Rect:
		useResourceName = TextureLibrary::GetTexture(jsonResource.try_emplace<std::string>("useResourceName"));
		break;
	default:
		break;
	}
	switch (drawType) {
	case ParticleDrawType::Mesh:
		drawSystemData = std::monostate();
		break;
	case ParticleDrawType::Rect:
	{
		drawSystemData = Rect{};
		auto& data = std::get<Rect>(drawSystemData);
		data.rect = jsonResource.try_emplace<Vector2>("Size");
		data.pivot = jsonResource.try_emplace<Vector2>("Pivot");
		break;
	}
	default:
		break;
	}
	jsonResource.register_value(SZG_JSON_ASSET_REGISTER(isLoop));
	jsonResource.register_value(SZG_JSON_ASSET_REGISTER(isParentEmitter));
	jsonResource.register_value(SZG_JSON_ASSET_REGISTER(duration));
	jsonResource.register_value(SZG_JSON_ASSET_REGISTER(emission));
	jsonResource.register_value(SZG_JSON_ASSET_REGISTER(particleInit));
	jsonResource.register_value(SZG_JSON_ASSET_REGISTER(particleFinal));
	create_draw_system();
}

ParticleEmitterInstance::~ParticleEmitterInstance() {
	// 登録解除

}

void ParticleEmitterInstance::update() {
	if (!is_active()) {
		return;
	}

	timer.ahead();
	if (timer > emission.delay) {
		cycleTimer.ahead();
	}

	if (isLoop && is_end()) {
		restart();
	}
	// パティクルの更新
	for (Reference<Particle>& particle : particles) {
		if (particle->is_active()) {
			particle->update();
		}
	}
	// 削除
	particles.remove_if(
		[&](Reference<Particle>& particle) {
		if (particle->is_ended()) {
			Reference<WorldRoot> manager = world_root_mut();
			manager->destroy(particle);
			return true;
		}
		return false;
	});
	// 生成
	if (!is_end() && cycleTimer.just_crossed(emission.interval) && emittedCycle < emission.cycles) {
		emit();
	}
}

void ParticleEmitterInstance::transfer() {
	if (!is_active() || !drawSystem || is_end_all()) {
		return;
	}
	for (u32 index = 0; Reference<Particle>& particle : particles) {
		drawSystem->write_to_buffer(
			index,
			particle->world_affine().to_matrix(),
			particle->create_uv_matrix(),
			particle->get_color()
		);
		++index;
	}
}

void ParticleEmitterInstance::draw() const {
	if (!is_active() || !drawSystem || is_end_all()) {
		return;
	}

	drawSystem->draw_command(particles.size());
}

void ParticleEmitterInstance::restart() {
	timer.set(0);
	cycleTimer.set(0);
	emittedCycle = 0;
}

void ParticleEmitterInstance::emit() {
	cycleTimer.set(0);
	++emittedCycle;
	for (u32 i = 0; i < emission.count; ++i) {
		size_t numParticle = particles.size();
		if (numParticle < numMaxParticle) {
			emit_once();
		}
	}
}

void ParticleEmitterInstance::emit_once() {
	// 位置
	Vector3 offset;
	switch (emission.shape.shapeType) {
	case Emission::Shape::ShapeType::Point:
		offset = CVector3::ZERO;
		break;
	case Emission::Shape::ShapeType::Sphere:
	{
		const auto& data = std::get<ParticleEmitterInstance::Emission::Shape::Sphere>(emission.shape.data);
		r32 cos = -2.0f * RandomEngine::Random01MOD() + 1.0f;
		r32 sin = std::sqrt(1.0f - cos * cos);
		r32 phi = PI2 * RandomEngine::Random01MOD();
		r32 radius = std::cbrt(RandomEngine::Random01MOD()) * data.radius;
		offset = { sin * std::cos(phi), sin * std::sin(phi), cos };
		offset *= radius;
		break;
	}
	case Emission::Shape::ShapeType::Cone:
	{
		const auto& data = std::get<ParticleEmitterInstance::Emission::Shape::Cone>(emission.shape.data);
		r32 maxCos = std::cos(data.angle);
		r32 cos = maxCos + (1 - maxCos) * RandomEngine::Random01MOD();
		r32 sin = std::sqrt(1.0f - cos * cos);
		r32 phi = PI2 * RandomEngine::Random01MOD();
		r32 radius = std::cbrt(RandomEngine::Random01MOD()) * data.radius;
		Vector3 base = { sin * std::cos(phi), sin * std::sin(phi), cos };
		Quaternion rotation1 = Quaternion::FromToRotation(CVector3::FORWARD, base);
		Quaternion rotation2 = Quaternion::FromToRotation(CVector3::FORWARD, data.direction);
		offset = CVector3::FORWARD * (rotation2 * rotation1) * radius;
		break;
	}
	case Emission::Shape::ShapeType::Box:
	{
		const auto& data = std::get<ParticleEmitterInstance::Emission::Shape::Box>(emission.shape.data);
		Vector3 base = {
			std::lerp(0.0f, data.size.x, RandomEngine::Random01MOD()),
			std::lerp(0.0f, data.size.y, RandomEngine::Random01MOD()),
			std::lerp(0.0f, data.size.z, RandomEngine::Random01MOD()),
		};
		offset = base - Vector3::Multiply(data.size, data.offset);
		break;
	}
	default:
		break;
	}
	// 方向
	r32 speed = std::lerp(particleInit.speed.min, particleInit.speed.max, RandomEngine::Random01Closed());
	Vector3 direction;
	switch (particleInit.direction.mode) {
	case ParticleInit::Direction::Mode::Constant:
	{
		const auto& data = std::get<ParticleEmitterInstance::ParticleInit::Direction::Constant>(particleInit.direction.data);
		direction = data.direction;
		break;
	}
	case ParticleInit::Direction::Mode::EmissionShape:
		direction = offset.normalize_safe();
		break;
	case ParticleInit::Direction::Mode::AngleRange:
	{
		const auto& data = std::get<ParticleEmitterInstance::ParticleInit::Direction::AngleRange>(particleInit.direction.data);
		r32 angle = std::lerp(0.0f, data.angle, RandomEngine::Random01Closed());
		r32 phi = std::lerp(0.0f, PI2, RandomEngine::Random01Closed());
		direction = data.baseDirection *
			Quaternion::AngleAxis(CVector3::UP, phi) * Quaternion::AngleAxis(CVector3::FORWARD, angle);
	}
	break;
	default:
		break;
	}
	// 回転
	std::variant<Particle::Constant, std::monostate, Particle::Random, Particle::LookAtAngle> rotation{ std::monostate{} };
	switch (particleInit.rotation.mode) {
	case Particle::RotationType::Constant:
	{
		const auto& data = std::get<ParticleInit::Rotation::Constant>(particleInit.rotation.data);
		rotation = Particle::Constant{ data.rotation };
		break;
	}
	case Particle::RotationType::Velocity:
	case Particle::RotationType::LookAt:
		break;
	case Particle::RotationType::LookAtAngle:
	{
		const auto& data = std::get<ParticleInit::Rotation::LookAtAngle>(particleInit.rotation.data);
		r32 temp = std::lerp(data.angleParSec.min, data.angleParSec.max, RandomEngine::Random01Closed());
		if (data.isRandomDirection && RandomEngine::Random01Bit<bool>()) {
			temp *= -1;
		}
		rotation = Particle::LookAtAngle{
			.angleParSec = temp
		};
	}
	break;
	case Particle::RotationType::Random:
	{
		const auto& data = std::get<ParticleInit::Rotation::Random>(particleInit.rotation.data);
		r32 cos = -2.0f * RandomEngine::Random01MOD() + 1.0f;
		r32 sin = std::sqrt(1.0f - cos * cos);
		r32 phi = PI2 * RandomEngine::Random01MOD();
		Vector3 axis = { sin * std::cos(phi), sin * std::sin(phi), cos };
		rotation = Particle::Random{
			.axis = axis.normalize_safe(),
			.angle = std::lerp(data.angularVelocity.min, data.angularVelocity.max, RandomEngine::Random01Closed())
		};
		break;
	}
	default:
		break;
	}

	// 生成
	particles.emplace_back(
		world_root_mut()->instantiate<Particle>(
			isParentEmitter ? this : nullptr,
			isParentEmitter ? offset : world_position() + offset,
			std::lerp(particleInit.lifetime.min, particleInit.lifetime.max, RandomEngine::Random01Closed()),
			direction * speed,
			Vector3::LerpElement(particleInit.acceleration.min, particleInit.acceleration.max,
				{ RandomEngine::Random01Closed(), RandomEngine::Random01Closed(), RandomEngine::Random01Closed() }),
			Color4::LerpElement(particleInit.color.min, particleInit.color.max,
				{ RandomEngine::Random01Closed(), RandomEngine::Random01Closed(), RandomEngine::Random01Closed(), RandomEngine::Random01Closed() }),
			Color4::LerpElement(particleFinal.color.min, particleFinal.color.max,
				{ RandomEngine::Random01Closed(), RandomEngine::Random01Closed(), RandomEngine::Random01Closed(), RandomEngine::Random01Closed() }),
			Vector3::LerpElement(particleInit.size.min, particleInit.size.max,
				{ RandomEngine::Random01Closed(), RandomEngine::Random01Closed(), RandomEngine::Random01Closed() }),
			Vector3::LerpElement(particleFinal.size.min, particleFinal.size.max,
				{ RandomEngine::Random01Closed(), RandomEngine::Random01Closed(), RandomEngine::Random01Closed() }),
			particleInit.rotation.mode, rotation
		)
	);
}

void ParticleEmitterInstance::on_mark_destroy() {
	for (Reference<Particle>& particle : particles) {
		world_root_mut()->destroy(particle);
	}
}

bool ParticleEmitterInstance::is_end_all() const {
	return is_end() && particles.empty();
}

bool ParticleEmitterInstance::is_end() const {
	return timer >= duration;
}

void ParticleEmitterInstance::create_draw_system() {
	drawSystem.reset();
	switch (drawType) {
	case ParticleDrawType::Mesh:
	{
		auto& val = std::get<0>(useResourceName);
		drawSystem = std::make_unique<ParticleDrawSystemMesh>(val);
		break;
	}
	case ParticleDrawType::Rect:
	{
		auto& val = std::get<1>(useResourceName);
		auto rectSystem = std::make_unique<ParticleDrawSystemRect>(val);
		const auto& data = std::get<Rect>(drawSystemData);
		rectSystem->create_rect(data.rect, data.pivot);
		drawSystem = std::move(rectSystem);
		break;
	}
	default:
		break;
	}
	drawSystem->create_buffers(numMaxParticle);
}

#ifdef DEBUG_FEATURES_ENABLE
//#include <imgui.h>
//void ParticleEmitterInstance::debug_gui() {
//	if (ImGui::CollapsingHeader("Emitter")) {
//		WorldInstance::debug_gui();
//	}
//	if (ImGui::CollapsingHeader("Particles")) {
//		ImGui::Text("Now/Max : %d/%d", particles.size(), numMaxParticle);
//	}
//	ImGui::Text("DrawType");
//	if (ImGui::RadioButton("Mesh", drawType == ParticleDrawType::Mesh)) {
//		drawType = ParticleDrawType::Mesh;
//		drawSystemData = std::monostate();
//		useResourceName = "";
//	}
//	ImGui::SameLine();
//	if (ImGui::RadioButton("Rect", drawType == ParticleDrawType::Rect)) {
//		drawType = ParticleDrawType::Rect;
//		drawSystemData = Rect{};
//		useResourceName = TextureLibrary::GetTexture("");
//	}
//	std::string_view resourceName;
//	switch (drawType) {
//	case ParticleDrawType::Mesh:
//	{
//		auto& val = std::get<0>(useResourceName);
//		resourceName = val;
//		ImGui::Text("Draw use : %s", val.c_str());
//		PolygonMeshLibrary::MeshListGui(val);
//		break;
//
//	}
//	case ParticleDrawType::Rect:
//	{
//		auto& val = std::get<1>(useResourceName);
//		resourceName = val->name();
//		ImGui::Text("Draw use : %s", resourceName.data());
//		TextureLibrary::TextureListGui(val);
//		auto& data = std::get<Rect>(drawSystemData);
//
//		ImGui::DragFloat2("Size", &data.rect.x, 0.1f, 0.0f, 1e10f);
//		ImGui::DragFloat2("Pivot", &data.pivot.x, 0.01f);
//		break;
//	}
//	default:
//		break;
//	}
//	if (ImGui::Button("Apply")) {
//		create_draw_system();
//	}
//	ImGui::Separator();
//	jsonResource.editor_gui();
//	if (ImGui::Button("Save")) {
//		jsonResource.write("DrawType", drawType);
//		jsonResource.write("useResourceName", resourceName);
//		if (drawType == ParticleDrawType::Rect) {
//			const auto& data = std::get<Rect>(drawSystemData);
//			jsonResource.write("Size", data.rect);
//			jsonResource.write("Pivot", data.pivot);
//		}
//		jsonResource.save();
//	}
//}

u32 ParticleEmitterInstance::ParticleInit::debug_gui(string_literal tag) {
	constexpr r32 FLOAT_MAX = std::numeric_limits<r32>::max();
	if (ImGui::TreeNode(tag)) {
		ImGui::DragFloatRange2("Lifetime", &lifetime.min, &lifetime.max, 0.01f, 0.0f, FLOAT_MAX);
		ImGui::DragFloatRange2("Speed", &speed.min, &speed.max, 0.1f, 0.0f, FLOAT_MAX);

		ImGui::Separator();
		if (ImGui::TreeNode("Direction")) {
			if (ImGui::RadioButton("Constant", direction.mode == Direction::Mode::Constant)) {
				direction.mode = Direction::Mode::Constant;
				direction.data = Direction::Constant{};
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("EmissionShape", direction.mode == Direction::Mode::EmissionShape)) {
				direction.mode = Direction::Mode::EmissionShape;
				direction.data = std::monostate();
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("AngleRange", direction.mode == Direction::Mode::AngleRange)) {
				direction.mode = Direction::Mode::AngleRange;
				direction.data = Direction::AngleRange{};
			}
			switch (direction.mode) {
			case Direction::Mode::Constant:
			{
				auto& data = std::get<ParticleEmitterInstance::ParticleInit::Direction::Constant>(direction.data);
				ImGui::DragFloat3("Direction", &data.direction.x, 0.01f, 0.0f, 1.0f);
				break;
			}
			case Direction::Mode::EmissionShape:
				// nothing
				break;
			case Direction::Mode::AngleRange:
			{
				auto& data = std::get<ParticleEmitterInstance::ParticleInit::Direction::AngleRange>(direction.data);
				ImGui::DragFloat3("Direction", &data.baseDirection.x, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Angle", &data.angle, 0.01f, 0.0f, PI);
				break;
			}
			default:
				break;
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
		ImGui::DragFloat3("AccelerationMin", &acceleration.min.x, 0.1f);
		ImGui::DragFloat3("AccelerationMax", &acceleration.max.x, 0.1f);
		ImGui::Separator();
		ImGui::DragFloat3("SizeMin", &size.min.x, 0.1f);
		ImGui::DragFloat3("SizeMax", &size.max.x, 0.1f);

		ImGui::Separator();
		if (ImGui::TreeNode("Rotation")) {
			if (ImGui::RadioButton("Constant", rotation.mode == Particle::RotationType::Constant)) {
				rotation.mode = Particle::RotationType::Constant;
				rotation.data = Rotation::Constant{};
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("LookForward", rotation.mode == Particle::RotationType::Velocity)) {
				rotation.mode = Particle::RotationType::Velocity;
				rotation.data = std::monostate();
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("LookAt", rotation.mode == Particle::RotationType::LookAt)) {
				rotation.mode = Particle::RotationType::LookAt;
				rotation.data = std::monostate();
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("LookAtAngle", rotation.mode == Particle::RotationType::LookAtAngle)) {
				rotation.mode = Particle::RotationType::LookAtAngle;
				rotation.data = Rotation::LookAtAngle();
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Random", rotation.mode == Particle::RotationType::Random)) {
				rotation.mode = Particle::RotationType::Random;
				rotation.data = Rotation::Random();
			}
			switch (rotation.mode) {
			case Particle::RotationType::Constant:
			{
				auto& data = std::get<ParticleEmitterInstance::ParticleInit::Rotation::Constant>(rotation.data);
				ImGui::DragFloat3("RotateRadian", &data.radian.x, 0.1f);
				data.rotation = Quaternion::EulerRadian(data.radian);
				break;
			}
			case Particle::RotationType::Velocity:
			case Particle::RotationType::LookAt:
				break;
			case Particle::RotationType::LookAtAngle:
			{
				auto& data = std::get<ParticleEmitterInstance::ParticleInit::Rotation::LookAtAngle>(rotation.data);
				ImGui::DragFloatRange2("AngleParSec", &data.angleParSec.min, &data.angleParSec.max, 0.01f);
				ImGui::Checkbox("IsRandomDirection", &data.isRandomDirection);
				break;
			}
			case Particle::RotationType::Random:
			{
				auto& data = std::get<ParticleEmitterInstance::ParticleInit::Rotation::Random>(rotation.data);
				ImGui::DragFloatRange2("MinAnglerVelocity", &data.angularVelocity.min, &data.angularVelocity.max, 0.1f);
				break;
			}
			default:
				break;
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
		color.min.debug_gui("ColorMin");
		color.max.debug_gui("ColorMax");

		ImGui::TreePop();
	}

	return 0;
}

u32 ParticleEmitterInstance::ParticleFinal::debug_gui(string_literal tag) {
	if (ImGui::TreeNode(tag)) {
		ImGui::DragFloat3("SizeMin", &size.min.x, 0.1f);
		ImGui::DragFloat3("SizeMax", &size.max.x, 0.1f);
		ImGui::Separator();
		color.min.debug_gui("ColorMin");
		color.max.debug_gui("ColorMax");
		ImGui::TreePop();
	}
	return 0;
}

u32 ParticleEmitterInstance::Emission::debug_gui(string_literal tag) {
	constexpr r32 FLOAT_MAX = std::numeric_limits<r32>::max();
	if (ImGui::TreeNode(tag)) {
		ImGui::DragFloat("Delay", &delay, 0.1f, 0.0f, FLOAT_MAX);
		ImGui::InputScalar("Count", ImGuiDataType_U32, &count);
		ImGui::InputScalar("Cycles", ImGuiDataType_U32, &cycles);
		ImGui::DragFloat("Interval", &interval, 0.1f, 0.0f, FLOAT_MAX);
		ImGui::Separator();
		if (ImGui::TreeNode("Shape")) {
			if (ImGui::RadioButton("Point", shape.shapeType == Shape::ShapeType::Point)) {
				shape.shapeType = Shape::ShapeType::Point;
				shape.data = std::monostate();
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Sphere", shape.shapeType == Shape::ShapeType::Sphere)) {
				shape.shapeType = Shape::ShapeType::Sphere;
				shape.data = Shape::Sphere{};
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Cone", shape.shapeType == Shape::ShapeType::Cone)) {
				shape.shapeType = Shape::ShapeType::Cone;
				shape.data = Shape::Cone{};
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Box", shape.shapeType == Shape::ShapeType::Box)) {
				shape.shapeType = Shape::ShapeType::Box;
				shape.data = Shape::Box{};
			}
			switch (shape.shapeType) {
			case ParticleEmitterInstance::Emission::Shape::ShapeType::Point:
				break;
			case ParticleEmitterInstance::Emission::Shape::ShapeType::Sphere:
			{
				auto& data = std::get<ParticleEmitterInstance::Emission::Shape::Sphere>(shape.data);
				ImGui::DragFloat("Radius", &data.radius, 0.1f, 0.0f, FLOAT_MAX);
				break;
			}
			case ParticleEmitterInstance::Emission::Shape::ShapeType::Cone:
			{
				auto& data = std::get<ParticleEmitterInstance::Emission::Shape::Cone>(shape.data);
				ImGui::DragFloat("Radius", &data.radius, 0.1f, 0.0f, FLOAT_MAX);
				ImGui::DragFloat3("Direction", &data.direction.x, 0.1f, -1.0f, 1.0f);
				ImGui::DragFloat("Angle", &data.angle, 0.1f, 0.0f, PI);
				break;
			}
			case ParticleEmitterInstance::Emission::Shape::ShapeType::Box:
			{
				auto& data = std::get<ParticleEmitterInstance::Emission::Shape::Box>(shape.data);
				ImGui::DragFloat3("Size", &data.size.x, 0.1f, 0.0f, FLOAT_MAX);
				ImGui::DragFloat3("Offset", &data.offset.x, 0.1f, 0.0f, FLOAT_MAX);
				break;
			}
			default:
				break;
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	return 0;
}
#endif // _DEBUG
