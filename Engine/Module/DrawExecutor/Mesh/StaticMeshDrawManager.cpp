#include "StaticMeshDrawManager.h"

using namespace szg;

#include <execution>

#include "Engine/Assets/PolygonMesh/PolygonMeshLibrary.h"

void StaticMeshDrawManager::make_instancing(u32 layer, const std::string& meshName_, u32 maxInstance) {
	if (layer >= maxLayer) {
		return;
	}
	if (!PolygonMeshLibrary::IsRegistered(meshName_)) {
		return;
	}

	std::pair<u32, std::string> key = std::make_pair(layer, meshName_);
	auto [emplaced, result] = executors.try_emplace(
		key,
		PolygonMeshLibrary::GetPolygonMesh(meshName_), maxInstance
	);

	if (result) {
		layerExecutors[layer].emplace_back(emplaced->second);
	}
}

#ifdef DEBUG_FEATURES_ENABLE
#include "Engine/Module/World/Camera/CameraInstance.h"
#include "Engine/Debug/DebugValues/DebugValues.h"

#include <imgui.h>
void StaticMeshDrawManager::debug_gui() {
	PolygonMeshLibrary::MeshListGui(select);

	i32 step = 1;
	ImGui::PushItemWidth(80);
	ImGui::InputScalar("Layer", ImGuiDataType_U32, &d_layer, (void*)&step);  ImGui::SameLine();
	ImGui::InputScalar("MaxInstance", ImGuiDataType_U32, &d_maxInstance, (void*)&step);
	ImGui::PopItemWidth();

	if (ImGui::Button("Apply") && d_maxInstance >= 1) {
		make_instancing(d_layer, select, d_maxInstance);
	}

	//for (u32 i = 0; auto & data : layerExecutors) {
	//	if (ImGui::TreeNode(std::format("Layer{}", i).c_str())) {
	//		ImGui::Text(std::format("RegisteredInstance : {}", instances.size()).c_str());
	//		ImGui::Indent();
	//		for (const auto& [name, executor] : data.executors) {
	//			ImGui::Text(std::format("{} : {}/{}", name, executor.count(), executor.max_instance()).c_str());
	//		}
	//		ImGui::Unindent();
	//		ImGui::TreePop();
	//	}
	//	++i;
	//}
}
#endif // _DEBUG
