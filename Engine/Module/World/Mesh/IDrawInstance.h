#pragma once

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Module/World/WorldInstance/WorldInstance.h"

namespace szg {

template<typename KeyType>
class IDrawInstance : public WorldInstance {
public:
	IDrawInstance() noexcept;
	virtual ~IDrawInstance() noexcept;

	SZG_CLASS_MOVE_ONLY(IDrawInstance)

public:
	virtual void update_affine() override;

public:
	const u32& layer() const { return renderLayer; }
	void set_layer(u32 layer) { renderLayer = layer; }
	virtual bool is_draw() const;
	void set_draw(bool flag) { isDraw = flag; }
	const Affine& local_affine() const { return localAffine; }
	const KeyType& key_id() const { return keyID; }

protected:
	bool isDraw{ true };

	KeyType keyID{};

	u32 renderLayer{ 0 };

	Affine localAffine;
	Transform3D localTransform{}; // Transform
};

template<typename KeyType>
IDrawInstance<KeyType>::IDrawInstance()  noexcept :
	WorldInstance() {
}

template<typename KeyType>
IDrawInstance<KeyType>::~IDrawInstance() noexcept = default;

template<typename KeyType>
void IDrawInstance<KeyType>::update_affine() {
	WorldInstance::update_affine();
	localAffine = Affine::FromTransform3D(localTransform);
}

template<typename KeyType>
bool IDrawInstance<KeyType>::is_draw() const {
	return isDraw && is_active();
}

}; // szg
