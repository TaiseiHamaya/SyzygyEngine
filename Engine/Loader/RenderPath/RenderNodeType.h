#pragma once

enum class RenderNodeType {
	WorldLayer = 0,
	PostEffect,
	StaticTexture,
	Result,

	Max,

	Unknown = -1,
};

enum class PostEffectType {
	Unknown,
	Bloom,               // ブルーム
	ChromaticAberration, // 色収差
	GaussianBlur,        // ガウシアンブラー
	Grayscale,           // グレースケール
	LuminanceExtraction, // 輝度抽出
	TextureBlend4,       // テクスチャ合成4
	Outline,             // アウトライン
	RadialBlur,          // ラジアルブラー

	Max,
};
