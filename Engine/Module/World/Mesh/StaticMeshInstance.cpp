#include "StaticMeshInstance.h"

using namespace szg;

#include "Engine/Application/Logger.h"
#include "Engine/Assets/PolygonMesh/PolygonMesh.h"
#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"
#include "Engine/Assets/Texture/TextureLibrary.h"

StaticMeshInstance::StaticMeshInstance() noexcept :
	IMultiMeshInstance() {
}

StaticMeshInstance::StaticMeshInstance(const std::string& meshName_) noexcept(false) :
	StaticMeshInstance() {
	reset_mesh(meshName_);
}

StaticMeshInstance::~StaticMeshInstance() noexcept = default;

void StaticMeshInstance::reset_mesh(const std::string& meshName_) {
	// メッシュ情報の取得
	if (PolygonMeshLibrary::IsRegistered(meshName_)) {
		keyID = meshName_;
	}
	else {
		keyID = "ErrorObject.obj";
	}

	default_material();
}

void StaticMeshInstance::default_material() {
	std::shared_ptr<const PolygonMesh> mesh = PolygonMeshLibrary::GetPolygonMesh(keyID);

	materials.resize(mesh->material_count());

	for (i32 i = 0; auto & meshMaterial : materials) {
		// 色情報のリセット
		const auto* meshMaterialData = mesh->material_data(i);
		if (meshMaterialData) {
			// テクスチャ情報の取得
			meshMaterial.texture = TextureLibrary::GetTexture(meshMaterialData->textureFileName);
			// uv情報のリセット
			meshMaterial.uvTransform = meshMaterialData->defaultUV;
		}
		else {
			meshMaterial.texture = TextureLibrary::GetTexture("Error.png");
			meshMaterial.uvTransform = Transform2D();
			szgWarning("Material data is not found.");
		}
		++i;
	}
}
