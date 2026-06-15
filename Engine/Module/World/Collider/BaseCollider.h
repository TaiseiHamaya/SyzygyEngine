#pragma once

#include <string>

#include "Engine/Module/World/WorldInstance/WorldInstance.h"

namespace szg {

class BaseCollider : public WorldInstance {
public:
	BaseCollider();
	~BaseCollider() = default;

public:
	const std::string& group() const noexcept;

public:
	void set_group_name(const std::string& name);

#ifdef DEBUG_FEATURES_ENABLE
	Matrix4x4 debug_matrix() const;
#endif // _DEBUG


private:
	const std::string* groupName;

#ifdef DEBUG_FEATURES_ENABLE
protected:
	Matrix4x4 debugMatrix;
#endif // _DEBUG
};

/*
Colliderに関する仕様メモ

CallBack関数を設定すると、衝突時にbindしたが呼び出される
Enter, Exitが設定されていなく衝突した場合、OnCollisionが呼び出される

この判定はColliderインスタンスごとに判定が行われる

*/

}; // szg
