#include "Rect3d.h"

#include <Library/Math/VectorConverter.h>

using namespace szg;

Rect3d::Rect3d() noexcept :
	IPrimitiveInstance() {
}

Rect3d::~Rect3d() noexcept = default;

void Rect3d::initialize(const Vector2& size_, const Vector2& pivot_) {
	data.size = size_;
	data.pivot = pivot_;
}

void Rect3d::initialize(const Vector3& size_, const Vector3& pivot_) {
	data.size = Converter::ToVector2(size_);
	data.pivot = Converter::ToVector2(pivot_);
}

void Rect3d::set_flip_y(bool flag) {
	data.isFlipY = flag;
}
