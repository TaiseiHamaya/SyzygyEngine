#include "ParticleDrawSystemMesh.h"

#include "Engine/Assets/PolygonMesh/PolygonMesh.h"
#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"
#include "Engine/Assets/Texture/TextureAsset.h"
#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"

using namespace szg;

ParticleDrawSystemMesh::ParticleDrawSystemMesh(const std::string& meshName) {
	set_mesh(meshName);
}

void ParticleDrawSystemMesh::draw_command(size_t InstanceCount) const {
	auto& commandList = DxCommand::GetCommandList();
	if (mesh) {
		commandList->IASetVertexBuffers(0, 1, &mesh->get_vbv(0));
		commandList->IASetIndexBuffer(mesh->get_p_ibv(0));
		commandList->SetGraphicsRootDescriptorTable(0, particleBuffer.get_handle_gpu());
		commandList->SetGraphicsRootDescriptorTable(2, texture->handle());

		commandList->DrawIndexedInstanced(mesh->index_size(0), static_cast<UINT>(InstanceCount), 0, 0, 0);
	}
}

void ParticleDrawSystemMesh::set_mesh(const std::string& meshName) {
	mesh = PolygonMeshLibrary::GetPolygonMesh(meshName);
	if (mesh) {
		if (mesh->material_count()) {
			texture = TextureLibrary::GetTexture(mesh->material_data(0)->textureFileName);
		}
		else {
			texture = TextureLibrary::GetTexture("Error.png");
		}
	}
}
