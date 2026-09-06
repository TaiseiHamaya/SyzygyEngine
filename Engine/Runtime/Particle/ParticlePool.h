#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Library/Math/ColorRGBA.h>
#include <Library/Math/Quaternion.h>
#include <Library/Math/Vector2.h>
#include <Library/Math/Vector3.h>
#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

#include "./ParticleUpdaters/BillboardRotationUpdater.h"
#include "./ParticleUpdaters/ParticleUpdater.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"
#include "Engine/Module/World/Particle/ParticleInstance.h"

namespace szg {

class EmitterInstance;
class ParticleBillboardDrawManager;
class ParticleMeshDrawManager;
class ParticleUpdaterCollection;
class TextureAsset;
class PolygonMesh;

/// <summary>
/// 描画種別
/// </summary>
enum class ParticleDrawType : u32 {
	Billboard,
	Mesh,
};

/// <summary>
/// 上限超過時のポリシー
/// </summary>
enum class ParticleOverflowPolicy : u32 {
	Discard,
	ReuseOldest,
};

/// <summary>
/// 放出時に決定される1粒分の初期値
/// </summary>
struct ParticleSpawnInitial {
	r32 lifetime{ 1.0f };
	Vector3 position{ CVector3::ZERO };
	Quaternion rotation{ CQuaternion::IDENTITY };
	Vector3 scale{ CVector3::ONE };
	Vector3 scaleEnd{ CVector3::ONE };
	Vector3 velocity{ CVector3::ZERO };
	Vector3 acceleration{ CVector3::ZERO };
	r32 drag{ 0.0f };
	Vector3 gravity{ CVector3::ZERO };
	ColorRGBA colorStart{ CColorRGBA::WHITE };
	ColorRGBA colorEnd{ CColorRGBA::WHITE };
	ParticleEasingMode easing{ ParticleEasingMode::Linear };
	r32 billboardAngle{ 0.0f };
	r32 billboardAngularVelocity{ 0.0f };
	Vector3 randomAxis{ CVector3::BASIS_Y };
	r32 randomAngularVelocity{ 0.0f };
	Vector3 lookTarget{ CVector3::ZERO };
	u32 lookAtCameraId{ LOOKAT_INVALID_CAMERA_ID };
	Vector3 axisBillboardAxis{ CVector3::BASIS_Y };
	r32 axisBillboardAngle{ 0.0f };
	Vector2 uvOffset{ CVector2::ZERO };
	Vector2 uvSpeed{ CVector2::ZERO };
	u32 flipColumns{ 1 };
	u32 flipRows{ 1 };
	r32 flipFrameTime{ 0.1f };
	bool flipLoop{ true };
	r32 fadeIn{ 0.0f };
	r32 fadeOut{ 0.0f };
};

/// <summary>
/// 描画指定
/// </summary>
struct ParticleDrawSpec {
	ParticleDrawType drawType{ ParticleDrawType::Billboard };
	u32 layer{ 0 };
	BlendMode blend{ BlendMode::Alpha };
	std::string textureName{ "Error.png" };
	std::string meshName{ "ErrorObject.obj" };
	Vector2 pivot{ CVector2::HALF };
};

/// <summary>
/// 粒子スロットの保管・確保・回収を行う実行単位
/// </summary>
class ParticlePool {
public:
	ParticlePool() = default;
	~ParticlePool();

	SZG_CLASS_MOVE_ONLY(ParticlePool)

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="updaters_">ワールド共有の更新者群</param>
	/// <param name="owner_">対のエミッタ</param>
	/// <param name="capacity_">上限数</param>
	/// <param name="policy_">超過ポリシー</param>
	void setup(Reference<ParticleUpdaterCollection> updaters_, Reference<EmitterInstance> owner_, u32 capacity_, ParticleOverflowPolicy policy_);

	/// <summary>
	/// 描画指定の設定
	/// </summary>
	/// <param name="spec_">描画指定</param>
	void setup_draw_spec(const ParticleDrawSpec& spec_);

	/// <summary>
	/// 更新者マスクと回転選択の設定
	/// </summary>
	/// <param name="mask_">更新者マスク</param>
	/// <param name="rotation_">回転選択</param>
	void setup_updaters(u32 mask_, ParticleRotationKind rotation_);

public:
	/// <summary>
	/// 空きスロットを確保し、各更新者へ登録する
	/// </summary>
	/// <param name="initial">初期値</param>
	/// <returns>確保成功時true</returns>
	bool try_spawn(const ParticleSpawnInitial& initial);

	/// <summary>
	/// 終了粒を回収する
	/// </summary>
	void reclaim();

	/// <summary>
	/// 全スロットの解除
	/// </summary>
	void clear();

	/// <summary>
	/// 専用描画バッファへの同期
	/// </summary>
	/// <param name="billboardManager">ビルボード描画管理</param>
	/// <param name="meshManager">メッシュ描画管理</param>
	void sync_draw(ParticleBillboardDrawManager& billboardManager, ParticleMeshDrawManager& meshManager);

public:
	/// <summary>
	/// 稼働数
	/// </summary>
	/// <returns></returns>
	u32 alive_count() const { return aliveCount; }

	/// <summary>
	/// 上限数
	/// </summary>
	/// <returns></returns>
	u32 max() const { return capacity; }

	/// <summary>
	/// 超過ポリシー
	/// </summary>
	/// <returns></returns>
	ParticleOverflowPolicy overflow_policy() const { return overflowPolicy; }

	/// <summary>
	/// 更新者マスク
	/// </summary>
	/// <returns></returns>
	u32 updater_mask() const { return updaterMask; }

	/// <summary>
	/// 回転選択
	/// </summary>
	/// <returns></returns>
	ParticleRotationKind rotation_kind() const { return rotationKind; }

	/// <summary>
	/// 稼働スロット番号列
	/// </summary>
	/// <returns></returns>
	const std::vector<u32>& alive_indices() const { return aliveIndices; }

	/// <summary>
	/// スロットの取得
	/// </summary>
	/// <param name="slotIndex">スロット番号</param>
	/// <returns></returns>
	ParticleInstance& instance_at(u32 slotIndex) { return slots[slotIndex].ids; }
	const ParticleInstance& instance_at(u32 slotIndex) const { return slots[slotIndex].ids; }

	/// <summary>
	/// 描画指定の取得
	/// </summary>
	/// <returns></returns>
	const ParticleDrawSpec& draw_spec_imm() const { return drawSpec; }

	/// <summary>
	/// 解決済みテクスチャの取得
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<const TextureAsset> texture_imm() const { return texture; }

	/// <summary>
	/// 解決済みメッシュの取得
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<const PolygonMesh> mesh_imm() const { return mesh; }

	/// <summary>
	/// 所有者が破棄予定か
	/// </summary>
	/// <returns></returns>
	bool is_owner_destroyed() const;

private:
	struct Slot {
		ParticleInstance ids{};
		bool alive{ false };
		u32 alivePosition{ 0 };
	};

	/// <summary>
	/// スロットの登録解除
	/// </summary>
	/// <param name="slotIndex">スロット番号</param>
	void unregister_slot(u32 slotIndex);

	/// <summary>
	/// 稼働列からの除去
	/// </summary>
	/// <param name="slotIndex">スロット番号</param>
	void remove_alive(u32 slotIndex);

private:
	std::vector<Slot> slots;
	std::vector<u32> freeList;
	std::vector<u32> aliveIndices;
	u32 aliveCount{ 0 };
	u32 capacity{ 0 };
	u32 reuseCursor{ 0 };
	ParticleOverflowPolicy overflowPolicy{ ParticleOverflowPolicy::Discard };
	u32 updaterMask{ 0xFFFFFFFFu };
	ParticleRotationKind rotationKind{ ParticleRotationKind::Billboard };
	ParticleDrawSpec drawSpec{};

	Reference<ParticleUpdaterCollection> updaters;
	Reference<EmitterInstance> owner;

	std::shared_ptr<const TextureAsset> texture;
	std::shared_ptr<const PolygonMesh> mesh;
};

}; // szg
