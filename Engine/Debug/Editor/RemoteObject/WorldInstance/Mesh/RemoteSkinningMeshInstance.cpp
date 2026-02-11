#ifdef DEBUG_FEATURES_ENABLE

#include "RemoteSkinningMeshInstance.h"

using namespace szg;

#include "../../../Window/EditorSceneView.h"
#include "Engine/Application/Logger.h"
#include "Engine/Assets/Animation/Skeleton/SkeletonAsset.h"
#include "Engine/Assets/PolygonMesh/PolygonMesh.h"
#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"
#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/Debug/Editor/Command/EditorCommandResizeContainer.h"
#include "Engine/Debug/Editor/Command/EditorCommandScope.h"
#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"
#include "Engine/Debug/Editor/Core/EditorAssetContentsCollector.h"

#define TRANSFORM2D_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

RemoteSkinningMeshInstance::RemoteSkinningMeshInstance() noexcept {
	debugVisual = std::make_unique<StaticMeshInstance>();
}

szg::RemoteSkinningMeshInstance::~RemoteSkinningMeshInstance() noexcept = default;

void RemoteSkinningMeshInstance::setup() {
	on_spawn();
	debugVisual->reset_mesh(meshName);
	if (sceneView) {
		sceneView->register_mesh(query_world(), debugVisual);
	}

	RemoteInstanceType::setup();
}

void RemoteSkinningMeshInstance::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	RemoteInstanceType::update_preview(world, parentAffine);

	debugVisual->reset_mesh(meshName);
	debugVisual->localAffine = worldAffine;
	debugVisual->isDraw = isDraw.value_imm();

	if (debugVisual->keyID != meshName.value_imm()) {
		return;
	}
	for (i32 i = 0; i < materials.size(); ++i) {
		RemoteSkinningMeshInstance::Material& source = materials[i];
		StaticMeshInstance::Material& write = debugVisual->materials[i];
		write.texture = TextureLibrary::GetTexture(source.texture);
		write.color = source.color;
		write.uvTransform = source.uvTransform;
		write.lightingType = source.lightingType;
		write.shininess = source.shininess;
	};
}

void RemoteSkinningMeshInstance::draw_inspector() {
	ImGui::Text("Type : SkinningMeshInstance");

	hierarchyName.show_gui();
	isUseRuntime.show_gui();

	ImGui::Separator();

	// Transform
	transform.show_gui();

	ImGui::Separator();

	isDraw.show_gui();
	layer.show_gui();
	if (meshName.show_gui().any()) {
		default_material();
	}

	if (ImGui::Button("ResetMaterialData")) {
		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());
		default_material();
		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
	}

	ImGui::Separator();
	// Material
	ImGui::Text("Materials");

	ValueEditor::show_object<Transform2D> uvTransformSO{ "UVTransform" };
	ValueEditor::show_object<ColorRGB> colorSO{ "Color" };
	ValueEditor::show_object<r32> shininessSO{ "Shininess" };

	for (i32 i = 0; auto& meshMaterial : materials) {
		std::string treeNodeName;
		auto meshData = PolygonMeshLibrary::GetPolygonMesh(meshName)->mesh_data(i);
		if (meshData) {
			treeNodeName = std::format("{}##{}", meshData->materialName, i);
		}
		if (treeNodeName.empty()) {
			treeNodeName = "UnknownMaterialName##" + std::to_string(i);
		}
		if (ImGui::TreeNodeEx(treeNodeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllColumns)) {
			{
				auto result = EditorAssetContentsCollector::ComboGUI(meshMaterial.texture, AssetType::Texture);

				if (result.has_value()) {
					EditorValueChangeCommandHandler::GenCommandInstant(materials, i, &Material::texture, result.value());
				}
			}

			{
				auto result = uvTransformSO.show_gui(meshMaterial.uvTransform);
				if (result == 0b01) {
					EditorValueChangeCommandHandler::GenCommand(materials, i, &Material::uvTransform);
				}
				else if (result == 0b10) {
					EditorValueChangeCommandHandler::End();
				}

			}

			{
				auto result = colorSO.show_gui(meshMaterial.color);
				if (result == 0b01) {
					EditorValueChangeCommandHandler::GenCommand(materials, i, &Material::color);
				}
				else if (result == 0b10) {
					EditorValueChangeCommandHandler::End();
				}
			}

			if (ImGui::RadioButton("None", meshMaterial.lightingType == LighingType::None)) {
				if (meshMaterial.lightingType != LighingType::None) {
					EditorValueChangeCommandHandler::GenCommandInstant<LighingType>(materials, i, &Material::lightingType, LighingType::None);
				}
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Lambert", meshMaterial.lightingType == LighingType::Lambert)) {
				if (meshMaterial.lightingType != LighingType::Lambert) {
					EditorValueChangeCommandHandler::GenCommandInstant<LighingType>(materials, i, &Material::lightingType, LighingType::Lambert);
				}
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Half lambert", meshMaterial.lightingType == LighingType::HalfLambert)) {
				if (meshMaterial.lightingType != LighingType::HalfLambert) {
					EditorValueChangeCommandHandler::GenCommandInstant<LighingType>(materials, i, &Material::lightingType, LighingType::HalfLambert);
				}
			}

			{
				auto result = shininessSO.show_gui(meshMaterial.shininess);
				if (result == 0b01) {
					EditorValueChangeCommandHandler::GenCommand(materials, i, &Material::lightingType);
				}
				else if (result == 0b10) {
					EditorValueChangeCommandHandler::End();
				}
			}

			ImGui::TreePop();
		}
		++i;
	}
	ImGui::Separator();

	// Animation
	animationName.show_gui();
	isLoop.show_gui();
	ImGui::Separator();

	// Skeleton
	if (skeleton) {
		ImGui::Text("NumSkeleton : %d", skeleton->joint_size());
	}
	else {
		ImGui::Text("Warning: Skeleton is not found.");
	}
}

nlohmann::json RemoteSkinningMeshInstance::serialize() const {
	nlohmann::json result;

	result.update(hierarchyName);
	result.update(isUseRuntime);
	result.update(transform);
	result["Type"] = instance_type();
	result.update(isDraw);
	result.update(layer);
	result.update(meshName);
	result["Materials"] = nlohmann::json::array();
	for (const auto& material : materials) {
		nlohmann::json jMaterial;
		jMaterial["Texture"] = material.texture;
		jMaterial["Color"] = material.color;
		jMaterial["UV Transform"] = material.uvTransform;
		jMaterial["LightingType"] = static_cast<std::underlying_type_t<LighingType>>(material.lightingType);
		jMaterial["Shininess"] = material.shininess;
		result["Materials"].emplace_back(std::move(jMaterial));
	}

	result.update(animationName);
	result.update(isLoop);

	return result;
}

void RemoteSkinningMeshInstance::on_spawn() {
	auto world = query_world();
	auto result = sceneView->get_layer(world);
	debugVisual->set_layer(result.value_or(-1));
}

void RemoteSkinningMeshInstance::on_destroy() {
	debugVisual->set_layer(std::numeric_limits<u32>::max());
}

void RemoteSkinningMeshInstance::default_material() {
	std::shared_ptr<const PolygonMesh> mesh = PolygonMeshLibrary::GetPolygonMesh(meshName);

	EditorCommandInvoker::Execute(std::make_unique<EditorCommandResizeContainer<std::vector<Material>>>(
		materials, mesh->material_count()
	));

	for (i32 i = 0; i < static_cast<i32>(materials.size()); ++i) {
		// 色情報のリセット
		const auto* meshMaterialData = mesh->material_data(i);
		if (meshMaterialData) {
			EditorValueChangeCommandHandler::GenCommandInstant(materials, i, &Material::texture, meshMaterialData->textureFileName);
			EditorValueChangeCommandHandler::GenCommandInstant(materials, i, &Material::uvTransform, meshMaterialData->defaultUV);
		}
		else {
			EditorValueChangeCommandHandler::GenCommandInstant<std::string>(materials, i, &Material::texture, "Error.png");
			EditorValueChangeCommandHandler::GenCommandInstant(materials, i, &Material::uvTransform);

			szgWarning("Material data is not found.");
		}
		EditorValueChangeCommandHandler::GenCommandInstant(materials, i, &Material::lightingType, LighingType::HalfLambert);
		EditorValueChangeCommandHandler::GenCommandInstant(materials, i, &Material::color, CColorRGB::WHITE);
		EditorValueChangeCommandHandler::GenCommandInstant(materials, i, &Material::shininess, 50.0f);
	}
}

#endif // DEBUG_FEATURES_ENABLE
