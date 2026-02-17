#include "PrimitiveGeometryAsset.h"

#define VECTOR3_SERIALIZER

#include "Engine/Assets/Json/JsonSerializer.h"
#include "Engine/Assets/Json/JsonAsset.h"

using namespace szg;

PrimitiveGeometryAsset::PrimitiveGeometryAsset(std::filesystem::path fileName) {
	JsonAsset json{ fileName };
	std::vector<Vector3> vertices;
	std::vector<u32> indexes;

	if (json.cget().contains("Vertices")) {
		const nlohmann::json& jsonVertices = json.cget().at("Vertices");
		for (const auto& elem : jsonVertices) {
			vertices.emplace_back(elem.get<Vector3>());
		}
	}

	if (json.cget().contains("Indexes")) {
		const nlohmann::json& jsonIndexes = json.cget().at("Indexes");
		for (const auto& elem : jsonIndexes) {
			indexes.emplace_back(elem.get<u32>());
		}
	}

	if (!vertices.empty()) {
		vertexBuffer.write(vertices);
	}
	if (!indexes.empty()) {
		indexBuffer.write(indexes);
	}
}

PrimitiveGeometryAsset::PrimitiveGeometryAsset(std::vector<Vector3> vertices_, std::vector<u32> indices) :
	vertexBuffer(vertices_),
	indexBuffer(indices) {
}

const D3D12_VERTEX_BUFFER_VIEW& PrimitiveGeometryAsset::get_vbv() const {
	return vertexBuffer.get_vbv();
}

const D3D12_INDEX_BUFFER_VIEW* const PrimitiveGeometryAsset::get_p_ibv() const {
	return indexBuffer.get_p_ibv();
}

const UINT PrimitiveGeometryAsset::index_size() const {
	return indexBuffer.index_size();
}
