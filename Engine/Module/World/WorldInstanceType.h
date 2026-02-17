#pragma once

enum class InstanceType {
	ErrorInstance = 0, // エラー用

	WorldInstance = 1000,

	// 描画Instance
	StaticMeshInstance = 2000,
	SkinningMeshInstance,
	Rect3dInstance,
	StringRectInstance,

	// カメラ
	CameraInstance = 3000,

	// Collider
	AABBColliderInstance = 4000,
	SphereColliderInstance,

	// Light
	DirectionalLightInstance = 5000,
	PointLightInstance,

	DebugFolder = 9000, // フォルダ用
#ifdef DEBUG_FEATURES_ENABLE
	DebugScene,
	DebugWorldRoot,
#endif // DEBUG_FEATURES_ENABLE

};
