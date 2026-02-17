#include "ParticleDrawSystemRect.h"

#include "Engine/Assets/Texture/TextureAsset.h"
#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"

using namespace szg;

ParticleDrawSystemRect::ParticleDrawSystemRect(std::shared_ptr<const TextureAsset> texture_) {
	texture = texture_;
}

void ParticleDrawSystemRect::draw_command(size_t InstanceCount) const {
	auto& commandList = DxCommand::GetCommandList();
	if (texture && InstanceCount != 0) {
		commandList->IASetVertexBuffers(0, 1, &vertexBuffer.get_vbv());
		commandList->SetGraphicsRootDescriptorTable(0, particleBuffer.get_handle_gpu());
		commandList->SetGraphicsRootDescriptorTable(2, texture->handle());

		commandList->DrawInstanced(6, static_cast<UINT>(InstanceCount), 0, 0);
	}
}

void ParticleDrawSystemRect::create_rect(const Vector2& size, const Vector2& pivot) {
	std::vector<BillboardBuffer> vertexData(6);
	vertexData[0] = {
		Vector2::Multiply(size, {-pivot.x, 1 - pivot.y}),
		CVector2::BASIS_X
	};
	vertexData[1] = {
		Vector2::Multiply(size, {-pivot.x, -pivot.y}),
		CVector2::BASIS
	};
	vertexData[2] = {
		Vector2::Multiply(size, {1 - pivot.x, 1 - pivot.y}),
		CVector2::ZERO
	};
	vertexData[3] = {
		Vector2::Multiply(size, {-pivot.x, -pivot.y}),
		CVector2::BASIS
	};
	vertexData[4] = {
		Vector2::Multiply(size, {1 - pivot.x, -pivot.y}),
		CVector2::BASIS_Y
	};
	vertexData[5] = {
		Vector2::Multiply(size, {1 - pivot.x, 1 - pivot.y}),
		CVector2::ZERO
	};
	vertexBuffer.write(vertexData);
}

void ParticleDrawSystemRect::set_texture(const std::string& textureName) {
	texture = TextureLibrary::GetTexture(textureName);
}
