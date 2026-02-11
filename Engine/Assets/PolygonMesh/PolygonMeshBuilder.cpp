#include "PolygonMeshBuilder.h"

using namespace szg;

#include <algorithm>
#include <sstream>

#include <Library/Utility/Tools/SmartPointer.h>

#include "./PolygonMesh.h"
#include "./PolygonMeshLibrary.h"
#include "Engine/Application/Logger.h"
#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/IndexBuffer/IndexBuffer.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/VertexBuffer/VertexBuffer.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

bool LoadMeshAssimp(const std::filesystem::path& filePath, std::vector<PolygonMesh::MeshData>& meshData, std::unordered_map<std::string, PolygonMesh::MeshMaterialData>& materialData);

PolygonMeshBuilder::PolygonMeshBuilder(const std::filesystem::path& filePath_) {
	filePath = IAssetBuilder::ResolveFilePath(filePath_, "Models");
}

bool PolygonMeshBuilder::run() {
	bool result = false;
	szgInformation(L"Start load file-\'{}\'", filePath.native());

	std::vector<PolygonMesh::MeshData> meshData;
	std::unordered_map<std::string, PolygonMesh::MeshMaterialData> materialData;
	result = LoadMeshAssimp(filePath, meshData, materialData);

	if (!result) {
		return result;
	}

	std::wstring meshFileName = filePath.filename().native();
	for (i32 index = 0; auto& mesh : meshData) {
		mesh.vertices->get_resource()->SetName(std::format(L"VertexBuffer-{}({})", index, meshFileName).c_str());
		mesh.indexes->get_resource()->SetName(std::format(L"IndexBuffer-{}({})", index, meshFileName).c_str());
		++index;
	}

	meshResult = eps::CreateShared<PolygonMesh>(meshData, materialData);

	szgInformation("Succeeded.");
	return true;
}

void PolygonMeshBuilder::postprocess() {
	// Do nothing
}

void PolygonMeshBuilder::transfer() {
	PolygonMeshLibrary::Transfer(filePath.filename().string(), meshResult);
}

bool LoadMeshAssimp(const std::filesystem::path& filePath, std::vector<PolygonMesh::MeshData>& meshData, std::unordered_map<std::string, PolygonMesh::MeshMaterialData>& materialData) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.string().c_str(),
		aiProcess_FlipUVs |
		aiProcess_FlipWindingOrder |
		aiProcess_LimitBoneWeights
	);
	if (importer.GetException() || !scene) {
		szgError("Failed to load mesh file. File-\'{}\' Message-\'{}\'", filePath.string(), importer.GetErrorString());
		return false;
	}
	if (!scene->HasMeshes()) {
		szgError("Can't find mesh. File-\'{}\'", filePath.string());
		return false;
	}

	// あとで逆参照する用
	std::unordered_map<u32, std::string> materialNameFromIndex;

	// Material解析
	for (u32 materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			PolygonMesh::MeshMaterialData newMaterial;

			// テクスチャ名の書き込み
			aiString textureFilePath;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath) == AI_SUCCESS) {
				newMaterial.textureFileName = textureFilePath.C_Str();
				std::filesystem::path directory{ filePath.parent_path() };
				TextureLibrary::RegisterLoadQue(L".\\" / directory / newMaterial.textureFileName);
			}

			// UVTransformの取得
			aiUVTransform uvTransform;
			if (material->Get(AI_MATKEY_UVTRANSFORM(aiTextureType_DIFFUSE, 0), uvTransform) == AI_SUCCESS) {
				Vector2 scale{ uvTransform.mScaling.x, uvTransform.mScaling.y };
				Vector2 translate{ uvTransform.mTranslation.x, uvTransform.mTranslation.y };

				newMaterial.defaultUV.set_scale(scale);
				newMaterial.defaultUV.set_rotate(uvTransform.mRotation);
				newMaterial.defaultUV.set_translate(translate);
			}

			std::string materialName;
			aiString aiMaterialName;
			// マテリアル名の取得
			if (material->Get(AI_MATKEY_NAME, aiMaterialName) == AI_SUCCESS) {
				materialName = aiMaterialName.C_Str();
			}
			materialData.emplace(materialName, std::move(newMaterial));
			materialNameFromIndex.emplace(materialIndex, materialName);
		}
	}

	// メッシュ解析
	for (u32 meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {

		std::vector<VertexDataBuffer> vertices; // vertexBuffer用
		std::vector<u32> indexes; // indexBuffer用
		aiMesh* mesh = scene->mMeshes[meshIndex];
		// normalが存在しない、texcoordが存在しない場合はメッシュとして使用しない
		if (!mesh->HasNormals() || !mesh->HasTextureCoords(0)) {
			szgError("This mesh don't have normal or texcoord. File-\'{}\'", filePath.string());
			continue;
		}
		vertices.reserve(mesh->mNumVertices);
		// 頂点取得
		for (u32 vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
			VertexDataBuffer& vertex = vertices.emplace_back();
			vertex.position = { -position.x,position.y, position.z };
			vertex.normal = { -normal.x, normal.y, normal.z };
			vertex.texcoord = { texcoord.x, texcoord.y };
		}
		indexes.reserve(static_cast<size_t>(mesh->mNumFaces) * 3);
		// Index取得
		for (u32 faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			if (face.mNumIndices != 3) {
				continue;
			}

			for (u32 element = 0; element < face.mNumIndices; ++element) {
				u32 vertexIndex = face.mIndices[element];
				indexes.emplace_back(vertexIndex);
			}
		}
		// 作成
		PolygonMesh::MeshData& newMesh = meshData.emplace_back();
		// 転送
		newMesh.vertices = std::make_unique<Object3DVertexBuffer>(vertices);
		newMesh.indexes = std::make_unique<IndexBuffer>(indexes);
		// メッシュ名の取得
		newMesh.meshName = mesh->mName.C_Str();
		// Material名の取得
		if (materialNameFromIndex.contains(mesh->mMaterialIndex)) {
			newMesh.materialName = materialNameFromIndex.at(mesh->mMaterialIndex);
		}
	}

	return true;
}
