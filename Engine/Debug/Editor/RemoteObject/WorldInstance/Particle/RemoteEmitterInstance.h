#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IRemoteInstance.h"

#include <memory>
#include <string>

#include <Library/Math/ColorRGBA.h>
#include <Library/Math/Vector2.h>
#include <Library/Math/Vector3.h>

#include "./RemoteParticlePoolView.h"
#include "Engine/Debug/Editor/Adapter/Field/EditorAssetField.h"
#include "Engine/Module/World/Mesh/Primitive/Rect3d.h"
#include "Engine/Module/World/Particle/EmitterInstance.h"
#include "Engine/Module/World/Particle/ParticlePool.h"
#include "Engine/Module/World/Particle/ParticleUpdaterCollection.h"

namespace szg {

/// <summary>
/// エミッタの編集・表示プロキシ
/// </summary>
class RemoteEmitterInstance final : public IRemoteInstance<EmitterInstance, Rect3d> {
public:
	friend class EditorSceneSerializer;

public:
	RemoteEmitterInstance() = default;
	~RemoteEmitterInstance() override = default;

public:
	void setup() override;

	void update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) override;

	void draw_inspector() override;

	nlohmann::json serialize() const override;

	constexpr InstanceType instance_type() const { return InstanceType::EmitterInstance; }

	void on_spawn() override;

	void on_destroy() override;

public:
	/// <summary>
	/// 描画指定
	/// </summary>
	struct DrawSection {
		DrawSection() = default;

		SZG_CLASS_DEFAULT(DrawSection)

	public:
		EditorValueField<u32> drawType{ "DrawType", 0 };
		EditorAssetField textureName{ "Texture", AssetType::Texture, "Error.png" };
		EditorAssetField meshName{ "Mesh", AssetType::Mesh, "ErrorObject.obj" };
		EditorValueField<u32> blend{ "Blend", 1 };
		EditorValueField<u32> layer{ "Layer", 0 };
		EditorValueField<Vector2> pivot{ "Pivot", Vector2{ 0.5f, 0.5f } };
		EditorValueField<u32> overflowPolicy{ "OverflowPolicy", 0 };
		EditorValueField<u32> capacity{ "Capacity", 256 };
		EditorValueField<bool> followParent{ "FollowParent", true };
	};

	/// <summary>
	/// 放出スケジュール
	/// </summary>
	struct ScheduleSection {
		ScheduleSection() = default;

		SZG_CLASS_DEFAULT(ScheduleSection)

	public:
		EditorValueField<r32> delay{ "Delay", 0.0f };
		EditorValueField<r32> interval{ "Interval", 0.1f };
		EditorValueField<u32> count{ "Count", 8 };
		EditorValueField<u32> cycles{ "Cycles", 0 };
		EditorValueField<bool> infinite{ "Infinite", true };
	};

	/// <summary>
	/// 放出形状と初期方向
	/// </summary>
	struct EmissionSection {
		EmissionSection() = default;

		SZG_CLASS_DEFAULT(EmissionSection)

	public:
		EditorValueField<u32> shape{ "Shape", 0 };
		EditorValueField<r32> sphereRadius{ "SphereRadius", 1.0f };
		EditorValueField<r32> coneRadius{ "ConeRadius", 1.0f };
		EditorValueField<Vector3> coneDirection{ "ConeDirection", CVector3::BASIS_Y };
		EditorValueField<r32> coneAngle{ "ConeAngle", 0.5f };
		EditorValueField<Vector3> boxSize{ "BoxSize", CVector3::ONE };
		EditorValueField<Vector3> boxOffset{ "BoxOffset", CVector3::ZERO };
		EditorValueField<u32> directionMode{ "DirectionMode", 0 };
		EditorValueField<Vector3> constantDirection{ "ConstantDirection", CVector3::BASIS_Y };
		EditorValueField<Vector3> angleBaseDirection{ "AngleBaseDirection", CVector3::BASIS_Y };
		EditorValueField<r32> angleRange{ "AngleRange", 0.5f };
	};

	/// <summary>
	/// 初期値のばらつき
	/// </summary>
	struct InitialSection {
		InitialSection() = default;

		SZG_CLASS_DEFAULT(InitialSection)

	public:
		EditorValueField<r32> lifetimeMin{ "LifetimeMin", 1.0f };
		EditorValueField<r32> lifetimeMax{ "LifetimeMax", 1.0f };
		EditorValueField<r32> speedMin{ "SpeedMin", 1.0f };
		EditorValueField<r32> speedMax{ "SpeedMax", 1.0f };
		EditorValueField<Vector3> sizeMin{ "SizeMin", CVector3::ONE };
		EditorValueField<Vector3> sizeMax{ "SizeMax", CVector3::ONE };
		EditorValueField<Vector3> scaleEndMin{ "ScaleEndMin", CVector3::ONE };
		EditorValueField<Vector3> scaleEndMax{ "ScaleEndMax", CVector3::ONE };
		EditorValueField<ColorRGBA> colorMin{ "ColorMin", CColorRGBA::WHITE };
		EditorValueField<ColorRGBA> colorMax{ "ColorMax", CColorRGBA::WHITE };
		EditorValueField<ColorRGBA> colorEndMin{ "ColorEndMin", CColorRGBA::WHITE };
		EditorValueField<ColorRGBA> colorEndMax{ "ColorEndMax", CColorRGBA::WHITE };
		EditorValueField<u32> easing{ "Easing", 0 };
		EditorValueField<Vector3> accelerationMin{ "AccelerationMin", CVector3::ZERO };
		EditorValueField<Vector3> accelerationMax{ "AccelerationMax", CVector3::ZERO };
		EditorValueField<bool> useDragGravity{ "UseDragGravity", false };
		EditorValueField<r32> drag{ "Drag", 0.0f };
		EditorValueField<Vector3> gravity{ "Gravity", CVector3::ZERO };
	};

	/// <summary>
	/// 回転方式
	/// </summary>
	struct RotationSection {
		RotationSection() = default;

		SZG_CLASS_DEFAULT(RotationSection)

	public:
		EditorValueField<u32> rotationKind{ "RotationKind", 0 };
		EditorValueField<Vector3> constantRotation{ "ConstantRotation", CVector3::ZERO };
		EditorValueField<r32> angleMin{ "AngleMin", 0.0f };
		EditorValueField<r32> angleMax{ "AngleMax", 0.0f };
		EditorValueField<r32> angularVelocityMin{ "AngularVelocityMin", 0.0f };
		EditorValueField<r32> angularVelocityMax{ "AngularVelocityMax", 0.0f };
		EditorValueField<r32> randomAngularVelocityMin{ "RandomAngularVelocityMin", 0.0f };
		EditorValueField<r32> randomAngularVelocityMax{ "RandomAngularVelocityMax", 0.0f };
		EditorValueField<Vector3> lookTarget{ "LookTarget", CVector3::ZERO };
		EditorValueField<u32> lookAtCameraId{ "LookAtCameraId", LOOKAT_INVALID_CAMERA_ID };
		EditorValueField<Vector3> axisBillboardAxis{ "AxisBillboardAxis", CVector3::BASIS_Y };
		EditorValueField<r32> axisBillboardAngle{ "AxisBillboardAngle", 0.0f };
	};

	/// <summary>
	/// 任意機能
	/// </summary>
	struct OptionSection {
		OptionSection() = default;

		SZG_CLASS_DEFAULT(OptionSection)

	public:
		EditorValueField<bool> useUVScroll{ "UseUVScroll", false };
		EditorValueField<Vector2> uvSpeedMin{ "UVSpeedMin", CVector2::ZERO };
		EditorValueField<Vector2> uvSpeedMax{ "UVSpeedMax", CVector2::ZERO };
		EditorValueField<bool> useFlipbook{ "UseFlipbook", false };
		EditorValueField<u32> flipColumns{ "FlipColumns", 1 };
		EditorValueField<u32> flipRows{ "FlipRows", 1 };
		EditorValueField<r32> flipFrameTime{ "FlipFrameTime", 0.1f };
		EditorValueField<bool> flipLoop{ "FlipLoop", true };
		EditorValueField<bool> useAlphaFade{ "UseAlphaFade", false };
		EditorValueField<r32> fadeIn{ "FadeIn", 0.0f };
		EditorValueField<r32> fadeOut{ "FadeOut", 0.0f };
		EditorAssetField particleFile{ "ParticleFile", AssetType::Particle, "" };
	};

private:
	/// <summary>
	/// フィールド値からの設定構築
	/// </summary>
	/// <returns></returns>
	EmitterInstanceSettings build_settings() const;

	/// <summary>
	/// 設定値のフィールドへの反映
	/// </summary>
	/// <param name="settings_">エミッタ設定</param>
	void apply_settings(const EmitterInstanceSettings& settings_);

	/// <summary>
	/// プレビュー模擬の再構築
	/// </summary>
	void rebuild_preview();

	/// <summary>
	/// ファイルセクションの表示
	/// </summary>
	void inspect_file();

	/// <summary>
	/// パーティクルファイルからの読み込み
	/// </summary>
	void import_particles();

	/// <summary>
	/// コンボボックスによる選択
	/// </summary>
	/// <param name="field">対象フィールド</param>
	/// <param name="label">表示名</param>
	/// <param name="items">選択肢</param>
	/// <param name="itemCount">選択肢数</param>
	void draw_combo(EditorValueField<u32>& field, string_literal label, const string_literal items[], u32 itemCount);

	/// <summary>
	/// 描画セクションの表示
	/// </summary>
	void inspect_draw();

	/// <summary>
	/// スケジュールセクションの表示
	/// </summary>
	void inspect_schedule();

	/// <summary>
	/// 放出セクションの表示
	/// </summary>
	void inspect_emission();

	/// <summary>
	/// 初期値セクションの表示
	/// </summary>
	void inspect_initial();

	/// <summary>
	/// 回転セクションの表示
	/// </summary>
	void inspect_rotation();

	/// <summary>
	/// 任意機能セクションの表示
	/// </summary>
	void inspect_options();

	/// <summary>
	/// プレビューセクションの表示
	/// </summary>
	void inspect_preview();

private:
	DrawSection draw;
	ScheduleSection schedule;
	EmissionSection emission;
	InitialSection initial;
	RotationSection rotation;
	OptionSection options;

	EditorValueField<bool> isPreviewPlaying{ "PreviewPlaying", true };

	std::unique_ptr<EmitterInstance> previewEmitter;
	std::unique_ptr<ParticlePool> previewPool;
	ParticleUpdaterCollection previewUpdaters;
	RemoteParticlePoolView poolView;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
