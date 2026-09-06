#pragma once

#include <string>

#include <Library/Math/Affine.h>
#include <Library/Math/ColorRGBA.h>
#include <Library/Math/Matrix3x3.h>
#include <Library/Math/Vector2.h>
#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"

namespace szg {

/// <summary>
/// 専用描画バッファへの書込単位
/// </summary>
class ParticleDrawItem {
public:
	ParticleDrawItem() = default;
	~ParticleDrawItem() = default;

	SZG_CLASS_DEFAULT(ParticleDrawItem)

public:
	/// <summary>
	/// 描画対象か
	/// </summary>
	/// <returns></returns>
	bool is_draw() const { return enabled; }

	/// <summary>
	/// 破棄予定か
	/// </summary>
	/// <returns></returns>
	bool is_marked_destroy() const { return false; }

	/// <summary>
	/// レイヤー
	/// </summary>
	/// <returns></returns>
	const u32& layer() const { return renderLayer; }

	/// <summary>
	/// ブレンドキー
	/// </summary>
	/// <returns></returns>
	const BlendMode& key_id() const { return blend; }

	/// <summary>
	/// メッシュキー
	/// </summary>
	/// <returns></returns>
	const std::string& mesh_key() const { return meshName; }

public:
	bool enabled{ false };
	u32 renderLayer{ 0 };
	BlendMode blend{ BlendMode::Alpha };
	std::string meshName{};
	Affine world{ CAffine::IDENTITY };
	Vector2 size{ CVector2::BASIS };
	Vector2 pivot{ 0.5f, 0.5f };
	r32 billboardAngle{ 0.0f };
	ColorRGBA color{ CColorRGBA::WHITE };
	Matrix3x3 uvTransform{ CMatrix3x3::IDENTITY };
	u32 textureIndex{ 0 };
};

}; // szg
