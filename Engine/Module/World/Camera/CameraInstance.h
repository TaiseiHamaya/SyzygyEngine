#pragma once

#include <memory>

#include <Library/Math/Vector2.h>

#include "./ProjectionAdapter/ICameraProjection.h"
#include "Engine/Module/World/WorldInstance/WorldInstance.h"

namespace szg {

class CameraInstance : public WorldInstance {
	friend class RemoteCamera3DInstance;

public:

public:
	CameraInstance() = default;
	virtual ~CameraInstance() = default;

	SZG_CLASS_MOVE_ONLY(CameraInstance)

public:
	void setup(std::unique_ptr<ICameraProjection> projection_) noexcept;

	void update_affine() override;

public:
	const Affine& view_affine() const;
	const Matrix4x4& proj_matrix() const;
	const Matrix4x4& vp_matrix() const;

protected:
	void make_view_matrix();

public:
	static Matrix4x4 MakeViewportMatrix(const Vector2& origin, const Vector2& size, r32 minDepth = 0.0f, r32 maxDepth = 1.0f);

private:
	Affine viewAffine;
	Matrix4x4 projectionMatrix;
	Matrix4x4 vpMatrix;

	std::unique_ptr<ICameraProjection> projection;
};

}; // szg
