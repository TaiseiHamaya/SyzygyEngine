#pragma once

#include <memory>

#include <Library/Math/ColorRGBA.h>
#include <Library/Math/Transform2D.h>

#include "../IDrawInstance.h"
#include "Engine/Assets/Texture/TextureAsset.h"
#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"

namespace szg {

struct PrimitiveMaterial {
	std::shared_ptr<const TextureAsset> texture;
	ColorRGBA color;
	Transform2D uvTransform;
	LighingType lightingType{ LighingType::HalfLambert };
	r32 shininess{ 50 };
};

template<typename PrimitiveData>
class IPrimitiveInstance : public IDrawInstance<BlendMode> {
public:
	IPrimitiveInstance() noexcept;
	virtual ~IPrimitiveInstance() noexcept = default;

	SZG_CLASS_MOVE_ONLY(IPrimitiveInstance)

public:
	PrimitiveMaterial& get_material() { return material; }
	const PrimitiveMaterial& get_material() const { return material; }
	PrimitiveData& get_data() { return data; }
	const PrimitiveData& get_data() const { return data; }

protected:
	PrimitiveData data;
	PrimitiveMaterial material;
};

template<typename PrimitiveData>
inline IPrimitiveInstance<PrimitiveData>::IPrimitiveInstance() noexcept :
	IDrawInstance() {
}

}; // szg
