#include "CollisionCallbackManager.h"

using namespace szg;

void CollisionCallbackManager::begin_callback() {
	for (auto& [_, value] : collisionRecent) {
		value <<= 1;
	}
}

void CollisionCallbackManager::remove_marked_destroy() {
	std::erase_if(collisionRecent, [](const std::pair<const CollisionRecentKeyType&, const std::bitset<2>&> kv) {
		const auto& key = kv.first;
		const auto& value = kv.second;
		return value.none() || key.big_imm()->is_marked_destroy() || key.small_imm()->is_marked_destroy();
	});
}

void CollisionCallbackManager::callback(CallbackTarget lhs, CallbackTarget rhs, bool result) {
	CallbackMapKey callbackKey = CallbackMapKey(lhs->group(), rhs->group());
	if (!callbackFunctions.contains(callbackKey)) {
		return;
	}
	const CallbackFunctions& callbackFunction = callbackFunctions.at(callbackKey);

	CollisionRecentKeyType recentKey = CollisionRecentKeyType(lhs, rhs);
	if (!collisionRecent.contains(recentKey)) {
		collisionRecent.emplace(recentKey, 0b00);
	}

	auto& collisionObject = collisionRecent.at(recentKey);

	collisionObject.set(0, result);
	u32 state = collisionObject.to_ulong();
	// このフレーム衝突しているか
	switch (state) {
	case 0b00:
		// 前フレームも衝突していない
		break;
	case 0b01:
		// 今はじめて衝突
			// Enter判定
		if (callbackFunction.onEnter) {
			callbackFunction.onEnter(lhs, rhs);
		}
		else if (callbackFunction.onContinue) {
			callbackFunction.onContinue(lhs, rhs);
		}
		break;
	case 0b10:
		// Exit
		if (callbackFunction.onExit) {
			callbackFunction.onExit(lhs, rhs);
		}
		else if (callbackFunction.onContinue) {
			callbackFunction.onContinue(lhs, rhs);
		}
		break;
	case 0b11:
		// 連続衝突
		if (callbackFunction.onContinue) {
			callbackFunction.onContinue(lhs, rhs);
		}
		break;
	default:
		break;
	}
}

const std::unordered_map<CollisionCallbackManager::CallbackMapKey, CollisionCallbackManager::CallbackFunctions>& szg::CollisionCallbackManager::callback_functions_imm() const {
	return callbackFunctions;
}
