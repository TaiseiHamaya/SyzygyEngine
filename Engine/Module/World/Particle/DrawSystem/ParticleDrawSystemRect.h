#pragma once

#include "BaseParticleDrawSystem.h"

#include <memory>
#include <string>

#include <Library/Math/Vector2.h>

#include "Engine/GraphicsAPI/DirectX/DxResource/VertexBuffer/VertexBuffer.h"

namespace szg {

class CameraInstance;
class TextureAsset;

class ParticleDrawSystemRect final : public BaseParticleDrawSystem {
public:
	ParticleDrawSystemRect(std::shared_ptr<const TextureAsset> texture);
	~ParticleDrawSystemRect() = default;

public:
	struct BillboardBuffer {
		Vector2 vertex;
		Vector2 texcoord;
	};

public:
	void draw_command(size_t InstanceCount) const override;

public:
	void create_rect(const Vector2& size, const Vector2& pivot = CVector2::ZERO);
	void set_texture(const std::string& textureName);

private:
	std::shared_ptr<const TextureAsset> texture;

public:
	VertexBuffer<BillboardBuffer> vertexBuffer;
};

}; // szg
