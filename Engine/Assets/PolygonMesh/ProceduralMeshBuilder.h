#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <Library/Math/Transform2D.h>

#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"
#include "PolygonMesh.h"

namespace szg {

class ProceduralMeshBuilder {
public:
	struct SubmeshData {
		std::vector<VertexDataBuffer> vertices;
		std::vector<u32> indices;
		std::string materialName;
	};

	ProceduralMeshBuilder& add_submesh(
		std::vector<VertexDataBuffer> vertices,
		std::vector<u32> indices,
		const std::string& materialName
	);

	ProceduralMeshBuilder& set_material(
		const std::string& name,
		const std::string& textureFileName = "",
		const Transform2D& defaultUV = Transform2D{}
	);

	std::shared_ptr<PolygonMesh> build(std::string_view meshName = "<Unnamed>");

	std::shared_ptr<PolygonMesh> build_and_register(const std::string& meshName);

	static void unregister(const std::string& meshName);

private:
	std::vector<SubmeshData> submeshes_;
	std::unordered_map<std::string, PolygonMesh::MeshMaterialData> materialData_;
};

}; // szg
