#pragma once

#include <Library/Math/ColorRGBA.h>
#include <Library/Math/Vector2.h>
#include <Library/Math/Vector3.h>

#include "./ParticlePool.h"
#include "./ParticleUpdaters/BillboardRotationUpdater.h"

namespace szg {

/// <summary>
/// 放出形状
/// </summary>
enum class ParticleEmissionShapeKind : u32 {
	Point,
	Sphere,
	Cone,
	Box,
};

/// <summary>
/// 初期方向の決定方式
/// </summary>
enum class ParticleDirectionMode : u32 {
	Constant,
	EmissionShape,
	AngleRange,
};

/// <summary>
/// 放出スケジュール
/// </summary>
struct EmitterScheduleSettings {
	r32 delay{ 0.0f };
	r32 interval{ 0.1f };
	u32 count{ 8 };
	u32 cycles{ 0 };
	bool infinite{ true };
};

/// <summary>
/// 放出形状
/// </summary>
struct EmitterShapeSettings {
	ParticleEmissionShapeKind shape{ ParticleEmissionShapeKind::Point };
	r32 sphereRadius{ 1.0f };
	r32 coneRadius{ 1.0f };
	Vector3 coneDirection{ CVector3::BASIS_Y };
	r32 coneAngle{ 0.5f };
	Vector3 boxSize{ CVector3::ONE };
	Vector3 boxOffset{ CVector3::ZERO };
};

/// <summary>
/// 初期方向
/// </summary>
struct EmitterDirectionSettings {
	ParticleDirectionMode directionMode{ ParticleDirectionMode::Constant };
	Vector3 constantDirection{ CVector3::BASIS_Y };
	Vector3 angleBaseDirection{ CVector3::BASIS_Y };
	r32 angleRange{ 0.5f };
};

/// <summary>
/// 初期値のばらつき
/// </summary>
struct EmitterInitialSettings {
	r32 lifetimeMin{ 1.0f };
	r32 lifetimeMax{ 1.0f };
	r32 speedMin{ 1.0f };
	r32 speedMax{ 1.0f };
	Vector3 sizeMin{ CVector3::ONE };
	Vector3 sizeMax{ CVector3::ONE };
	Vector3 scaleEndMin{ CVector3::ONE };
	Vector3 scaleEndMax{ CVector3::ONE };
	ColorRGBA colorMin{ CColorRGBA::WHITE };
	ColorRGBA colorMax{ CColorRGBA::WHITE };
	ColorRGBA colorEndMin{ CColorRGBA::WHITE };
	ColorRGBA colorEndMax{ CColorRGBA::WHITE };
	ParticleEasingMode easing{ ParticleEasingMode::Linear };
	Vector3 accelerationMin{ CVector3::ZERO };
	Vector3 accelerationMax{ CVector3::ZERO };
};

/// <summary>
/// 減衰と重力
/// </summary>
struct EmitterMotionSettings {
	bool useDragGravity{ false };
	r32 drag{ 0.0f };
	Vector3 gravity{ CVector3::ZERO };
};

/// <summary>
/// 回転方式
/// </summary>
struct EmitterRotationSettings {
	ParticleRotationKind rotationKind{ ParticleRotationKind::Billboard };
	Vector3 constantRotation{ CVector3::ZERO };
	r32 angleMin{ 0.0f };
	r32 angleMax{ 0.0f };
	r32 angularVelocityMin{ 0.0f };
	r32 angularVelocityMax{ 0.0f };
	r32 randomAngularVelocityMin{ 0.0f };
	r32 randomAngularVelocityMax{ 0.0f };
	Vector3 lookTarget{ CVector3::ZERO };
	u32 lookAtCameraId{ LOOKAT_INVALID_CAMERA_ID };
	Vector3 axisBillboardAxis{ CVector3::BASIS_Y };
	r32 axisBillboardAngle{ 0.0f };
};

/// <summary>
/// UVスクロール
/// </summary>
struct EmitterUVSettings {
	bool useUVScroll{ false };
	Vector2 uvSpeedMin{ CVector2::ZERO };
	Vector2 uvSpeedMax{ CVector2::ZERO };
};

/// <summary>
/// フリップブック
/// </summary>
struct EmitterFlipbookSettings {
	bool useFlipbook{ false };
	u32 flipColumns{ 1 };
	u32 flipRows{ 1 };
	r32 flipFrameTime{ 0.1f };
	bool flipLoop{ true };
};

/// <summary>
/// 透明度フェード
/// </summary>
struct EmitterFadeSettings {
	bool useAlphaFade{ false };
	r32 fadeIn{ 0.0f };
	r32 fadeOut{ 0.0f };
};

/// <summary>
/// エミッタ設定
/// </summary>
struct EmitterInstanceSettings {
	ParticleDrawSpec drawSpec{};
	ParticleOverflowPolicy overflowPolicy{ ParticleOverflowPolicy::Discard };
	u32 capacity{ 256 };
	bool followParent{ true };
	EmitterScheduleSettings schedule{};
	EmitterShapeSettings emissionShape{};
	EmitterDirectionSettings emissionDirection{};
	EmitterInitialSettings initial{};
	EmitterMotionSettings motion{};
	EmitterRotationSettings rotation{};
	EmitterUVSettings uv{};
	EmitterFlipbookSettings flipbook{};
	EmitterFadeSettings fade{};
};

}; // szg
