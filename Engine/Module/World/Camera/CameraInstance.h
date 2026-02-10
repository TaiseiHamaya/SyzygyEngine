#pragma once

#include <memory>

#include <Library/Math/Vector2.h>

#include "./ProjectionAdapter/ICameraProjection.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Module/World/WorldInstance/WorldInstance.h"

namespace szg {

class CameraInstance : public WorldInstance {
	friend class RemoteCamera3DInstance;

public:
	struct VpBuffers {
		Matrix4x4 viewProjection;
		Matrix4x4 view;
	};

	struct LightingPathBuffer {
		Vector3 position;
		u32 padding{ 0 };
		Matrix4x4 viewInv;
		Matrix4x4 projInv;
	};

public:
	CameraInstance() = default;
	virtual ~CameraInstance() = default;

	SZG_CLASS_MOVE_ONLY(CameraInstance)

public:
	void setup(std::unique_ptr<ICameraProjection> projection_) noexcept;

	void update_affine() override;
	void transfer();

	void register_world_projection(u32 index) const;
	void register_world_lighting(u32 index) const;

public:
	const Matrix4x4& vp_matrix() const;

	const Affine& view_affine() const;
	const Matrix4x4& proj_matrix() const;

protected:
	void make_view_matrix();

public:
	static Matrix4x4 MakeViewportMatrix(const Vector2& origin, const Vector2& size, r32 minDepth = 0.0f, r32 maxDepth = 1.0f);

private:
	Affine viewAffine;
	Matrix4x4 projectionMatrix;

	ConstantBuffer<VpBuffers> vpBuffers;
	ConstantBuffer<LightingPathBuffer> lightingBuffer;

	std::unique_ptr<ICameraProjection> projection;
};

}; // szg
