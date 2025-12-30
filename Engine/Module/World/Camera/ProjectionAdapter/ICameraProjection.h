#pragma once

#include <Library/Math/Matrix4x4.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

class ICameraProjection {
public:
	ICameraProjection() = default;
	virtual ~ICameraProjection() = default;

	SZG_CLASS_MOVE_ONLY(ICameraProjection)

public:
	virtual Matrix4x4 generate_matrix() const noexcept = 0;
};
