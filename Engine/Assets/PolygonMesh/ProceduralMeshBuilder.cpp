#include "ProceduralMeshBuilder.h"

#include <format>

#include <Library/Utility/Tools/ConvertString.h>
#include <Library/Utility/Tools/SmartPointer.h>

#include "./PolygonMesh.h"
#include "./PolygonMeshLibrary.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/IndexBuffer/IndexBuffer.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/VertexBuffer/VertexBuffer.h"

using namespace szg;

ProceduralMeshBuilder& ProceduralMeshBuilder::add_submesh(
	std::vector<VertexDataBuffer> vertices,
	std::vector<u32> indices,
	const std::string& materialName) {
	submeshes_.emplace_back(std::move(vertices), std::move(indices), materialName);
	return *this;
}

ProceduralMeshBuilder& ProceduralMeshBuilder::set_material(
	const std::string& name,
	const std::string& textureFileName,
	const Transform2D& defaultUV) {
	materialData_[name] = PolygonMesh::MeshMaterialData{ textureFileName, defaultUV };
	return *this;
}

std::shared_ptr<PolygonMesh> ProceduralMeshBuilder::build(std::string_view meshName) {
	std::vector<PolygonMesh::MeshData> meshData;
	meshData.reserve(submeshes_.size());
	std::wstring meshNameW = ConvertString(meshName);
	for (i32 i = 0; auto& sub : submeshes_) {
		if (sub.indices.empty() || sub.vertices.empty()) {
			continue;
		}
		PolygonMesh::MeshData md;
		md.vertices = std::make_unique<Object3DVertexBuffer>(sub.vertices);
		md.vertices->get_resource()->SetName(std::format(L"VertexBuffer-{}({}-{})", i, meshNameW, L"Procedural").c_str());
		md.indexes = std::make_unique<IndexBuffer>(sub.indices);
		md.indexes->get_resource()->SetName(std::format(L"IndexBuffer-{}({}-{})", i, meshNameW, L"Procedural").c_str());
		md.meshName = sub.materialName;
		md.materialName = sub.materialName;
		meshData.emplace_back(std::move(md));

		++i;
	}
	return eps::CreateShared<PolygonMesh>(meshData, materialData_);
}

std::shared_ptr<PolygonMesh> ProceduralMeshBuilder::build_and_register(const std::string& meshName) {
	auto mesh = build(meshName);
	if (PolygonMeshLibrary::IsRegistered(meshName)) {
		PolygonMeshLibrary::Replace(meshName, mesh);
	}
	else {
		PolygonMeshLibrary::Transfer(meshName, mesh);
	}
	return mesh;
}

void ProceduralMeshBuilder::unregister(const std::string& meshName) {
	PolygonMeshLibrary::Unload(meshName);
}
