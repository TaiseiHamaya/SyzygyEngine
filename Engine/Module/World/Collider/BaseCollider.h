#pragma once

#include "Engine/Module/World/WorldInstance/WorldInstance.h"

namespace szg {

class BaseCollider : public WorldInstance {
public:
	BaseCollider() = default;
	~BaseCollider() = default;

public:
	i32 group_imm() const noexcept;

private:
	i32 colliderGroupId { 0 };
};

/*
Colliderに関する仕様メモ

CallBack関数を設定すると、衝突時にbindした関数が呼び出される
Enter/Exitが設定されていない場合にはOnCollisionが呼び出される

この判定はColliderインスタンスごとに判定が行われる

*/

}; // szg
