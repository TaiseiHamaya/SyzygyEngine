#ifdef DEBUG_FEATURES_ENABLE

#include "RemoteStaticMeshInstance.h"

using namespace szg;

#include "../../../Window/EditorSceneView.h"
#include "Engine/Application/Logger.h"
#include "Engine/Assets/PolygonMesh/PolygonMesh.h"
#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"
#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/Debug/Editor/Command/EditorCommandScope.h"
#include "Engine/Debug/Editor/Command/EditorCommandResizeContainer.h"
#include "Engine/Debug/Editor/Core/EditorAssetContentsCollector.h"

RemoteStaticMeshInstance::RemoteStaticMeshInstance() {
	debugVisual = std::make_unique<StaticMeshInstance>();
}

void RemoteStaticMeshInstance::setup() {
	on_spawn();
	debugVisual->reset_mesh(meshName);
	// Editor側でDrawExecutorに登録
	if (sceneView) {
		sceneView->register_mesh(query_world(), debugVisual);
	}

	IRemoteInstance<StaticMeshInstance, StaticMeshInstance>::setup();
}

void RemoteStaticMeshInstance::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	IRemoteInstance<StaticMeshInstance, StaticMeshInstance>::update_preview(world, parentAffine);

	debugVisual->reset_mesh(meshName);
	debugVisual->localAffine = worldAffine;
	debugVisual->isDraw = isDraw.value_imm();

	if (debugVisual->keyID != meshName.value_imm()) {
		return;
	}
	for (i32 i = 0; i < materials.size(); ++i) {
		RemoteStaticMeshInstance::Material& source = materials[i];
		StaticMeshInstance::Material& write = debugVisual->materials[i];
		write.texture = TextureLibrary::GetTexture(source.texture);
		write.color = source.color;
		write.uvTransform = source.uvTransform;
		write.lightingType = source.lightingType;
		write.shininess = source.shininess;
	};
}

void RemoteStaticMeshInstance::draw_inspector() {
	ImGui::Text("Type : StaticMeshInstance");

	hierarchyName.show_gui();
	isUseRuntime.show_gui();

	ImGui::Separator();
	// Transform
	transform.show_gui();
	ImGui::Separator();

	isDraw.show_gui();
	layer.show_gui();
	{
		std::string cache = meshName;
		if (PolygonMeshLibrary::MeshListGui(cache)) {
			if (cache != meshName.value_imm()) {
				EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());

				default_material();

				EditorValueChangeCommandHandler::GenCommandInstant<std::string>(meshName.value_mut(), cache);

				// Editor側のDrawExecutorに登録
				if (sceneView) {
					sceneView->create_mesh_instancing(query_world(), meshName);
				}

				default_material();

				EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
			}
		}
	}
	if (ImGui::Button("ResetMaterialData")) {
		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());
		default_material();
		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
	}

	ImGui::Separator();
	// Material
	ImGui::Text("Materials");
	ValueEditor::show_object<ColorRGB> colorSO{ "Color" };
	ValueEditor::show_object<Transform2D> uvTransformSO{ "UV Transform" };
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
					EditorValueChangeCommandHandler::GenCommandInstant<std::string>(materials, i, &Material::texture, result.value());
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
					EditorValueChangeCommandHandler::GenCommand(materials, i, &Material::lightingType);
					meshMaterial.lightingType = LighingType::None;
					EditorValueChangeCommandHandler::End();
				}
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Lambert", meshMaterial.lightingType == LighingType::Lambert)) {
				if (meshMaterial.lightingType != LighingType::Lambert) {
					EditorValueChangeCommandHandler::GenCommand(materials, i, &Material::lightingType);
					meshMaterial.lightingType = LighingType::Lambert;
					EditorValueChangeCommandHandler::End();
				}
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Half lambert", meshMaterial.lightingType == LighingType::HalfLambert)) {
				if (meshMaterial.lightingType != LighingType::HalfLambert) {
					EditorValueChangeCommandHandler::GenCommand(materials, i, &Material::lightingType);
					meshMaterial.lightingType = LighingType::HalfLambert;
					EditorValueChangeCommandHandler::End();
				}
			}

			{
				auto result = shininessSO.show_gui(meshMaterial.shininess);
				if (result == 0b01) {
					EditorValueChangeCommandHandler::GenCommand(materials, i, &Material::shininess);
				}
				else if (result == 0b10) {
					EditorValueChangeCommandHandler::End();
				}
			}

			ImGui::TreePop();
		}
		++i;
	}
}

nlohmann::json RemoteStaticMeshInstance::serialize() const {
	nlohmann::json json;

	json.update(hierarchyName);
	json.update(isUseRuntime);
	json.update(transform);
	json["Type"] = instance_type();
	json.update(isDraw);
	json.update(layer);
	json["MeshName"] = meshName;
	json["Materials"] = nlohmann::json::array();
	for (const auto& material : materials) {
		nlohmann::json jMaterial;
		jMaterial["Texture"] = material.texture;
		jMaterial["Color"] = material.color;
		jMaterial["UV Transform"] = material.uvTransform;
		jMaterial["LightingType"] = static_cast<std::underlying_type_t<LighingType>>(material.lightingType);
		jMaterial["Shininess"] = material.shininess;
		json["Materials"].emplace_back(std::move(jMaterial));
	}

	return json;
}

void RemoteStaticMeshInstance::on_spawn() {
	auto world = parent->query_world();
	auto result = sceneView->get_layer(world);
	debugVisual->set_layer(result.value_or(-1));

	IRemoteInstance<StaticMeshInstance, StaticMeshInstance>::on_spawn();
}

void RemoteStaticMeshInstance::on_destroy() {
	debugVisual->set_layer(std::numeric_limits<u32>::max());

	IRemoteInstance<StaticMeshInstance, StaticMeshInstance>::on_destroy();
}

void RemoteStaticMeshInstance::default_material() {
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

		EditorValueChangeCommandHandler::GenCommandInstant<LighingType>(materials, i, &Material::lightingType, LighingType::HalfLambert);
		EditorValueChangeCommandHandler::GenCommandInstant<ColorRGB>(materials, i, &Material::color, CColorRGB::WHITE);
		EditorValueChangeCommandHandler::GenCommandInstant(materials, i, &Material::shininess, 50.0f);
	}
}

#endif // DEBUG_FEATURES_ENABLE
