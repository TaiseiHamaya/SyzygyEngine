#include "PolygonMesh.h"

#include "Engine/GraphicsAPI/DirectX/DxResource/IndexBuffer/IndexBuffer.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/VertexBuffer/VertexBuffer.h"

using namespace szg;

PolygonMesh::PolygonMesh(std::vector<MeshData>& meshData_, std::unordered_map<std::string, MeshMaterialData>& materialData_) noexcept : 
	meshData(std::move(meshData_)),
	materialData(std::move(materialData_)) {
}

PolygonMesh::~PolygonMesh() noexcept = default;

const D3D12_VERTEX_BUFFER_VIEW& PolygonMesh::get_vbv(u32 index) const {
	return meshData[index].vertices->get_vbv();
}

const D3D12_INDEX_BUFFER_VIEW* const PolygonMesh::get_p_ibv(u32 index) const {
	return meshData[index].indexes->get_p_ibv();
}

const size_t PolygonMesh::material_count() const {
	return meshData.size();
}

const UINT PolygonMesh::index_size(u32 index) const {
	return static_cast<const UINT>(meshData[index].indexes->index_size());
}

const PolygonMesh::MeshData* PolygonMesh::mesh_data(u32 index) const {
	if (index < material_count()) {
		return &meshData[index];
	}
	return nullptr;
}

const PolygonMesh::MeshMaterialData* PolygonMesh::material_data(u32 index) const {
	if (index >= material_count()) {
		return nullptr;
	}
	if (has_material(index)) {
		return &materialData.at(meshData[index].materialName);
	}
	return nullptr;
}

bool PolygonMesh::has_material(u32 index) const {
	return materialData.contains(meshData[index].materialName);
}
