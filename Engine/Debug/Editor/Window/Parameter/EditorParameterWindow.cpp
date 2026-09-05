#ifdef DEBUG_FEATURES_ENABLE

#include "EditorParameterWindow.h"

#include <imgui.h>

#include "Engine/Assets/Json/JsonAsset.h"
#include "Engine/Debug/Editor/Command/EditorCommandResizeContainer.h"
#include "Engine/Debug/Editor/Command/EditorCommandScope.h"
#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"
#include "Engine/Debug/Editor/Command/ValueChangeCommandHelper/SparseSetDefaultAccesser.h"
#include "Engine/Debug/Editor/Core/EditorAssetContentsCollector.h"
#include "Engine/Debug/ImGui/ImGuiJsonEditor/ValueEditorObject.h"

#define VECTOR2_SERIALIZER
#define VECTOR3_SERIALIZER
#define COLOR_RGBA_SERIALIZER
#define COLOR_RGB_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"
#include "Engine/Assets/Json/Serializer/UnormSerializer.h"

void szg::EditorParameterWindow::draw() {
	if (!is_active()) {
		return;
	}

	update_focus();

	i32 flags = 0;
	ImGui::Begin("Parameter##ParamEditor", &isActive, flags);

	if (filename.has_value()) {
		if (stringEditBuffer.has_value() && isEditingFilename) {
			i32 textFlags =
				ImGuiInputTextFlags_AutoSelectAll |
				ImGuiInputTextFlags_EnterReturnsTrue |
				ImGuiInputTextFlags_EscapeClearsAll;
			ImGui::Text("   File: ");
			ImGui::SameLine();
			ImGui::SetKeyboardFocusHere();
			ImGui::SetNextItemWidth(250);
			ImGui::InputText("##EditorFilename", &stringEditBuffer.value(), textFlags);
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				EditorValueChangeCommandHandler::GenCommandInstant<decltype(filename)>(filename, stringEditBuffer.value() + ".param");
			}
			if (ImGui::IsItemDeactivated()) {
				stringEditBuffer.reset();
				isEditingFilename = false;
			}

			ImGui::SameLine();

			ImGui::Text(".param");
		}
		else {
			if (ImGui::Button("\ue14d")) {
				// copy path to clipboard
				if (filename.has_value()) {
					ImGui::SetClipboardText(filename.value().string().c_str());
				}
			}
			ImGui::SameLine();

			ImGui::Text("File: ");
			ImGui::SameLine();

			std::string displayText = std::format("{}##EditorFilenameDisplay", filename.value().string());
			r32 textWidth = ImGui::CalcTextSize(displayText.c_str(), nullptr, true).x;

			i32 selectableFlags =
				ImGuiSelectableFlags_AllowDoubleClick;
			if (ImGui::Selectable(displayText.c_str(), false, selectableFlags, ImVec2(textWidth, 0))) {
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					stringEditBuffer = filename.value().stem().string();
					isEditingFilename = true;
				}
			}
		}
	}
	else {
		ImGui::Text("File: None");
		ImGui::SameLine();

		if (ImGui::Button("\ue091##CreateFile")) {
			// create file
			stringEditBuffer = "";
			filename = "";
			isEditingFilename = true;
		}
	}

	if (ImGui::Button("\ue161")) {
		if (filename.has_value()) {
			save();
		}
	}

	std::string currentFileName = filename.has_value() ? filename.value().string() : "None";
	auto resultFileSelect = EditorAssetContentsCollector::ComboGUI(currentFileName, AssetType::Param);
	if (resultFileSelect.has_value()) {
		paramSelectedId.reset();
		stringEditBuffer.reset();

		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());

		EditorValueChangeCommandHandler::GenCommandInstant<decltype(filename)>(filename, resultFileSelect.value().path.filename());
		EditorValueChangeCommandHandler::GenCommandInstant<decltype(parameters)>(parameters, parameters);
		EditorCommandInvoker::Execute(
			std::make_unique<EditorCommandResizeContainer<decltype(parameters)>>(parameters, 0)
		);
		load();
		EditorValueChangeCommandHandler::GenCommandInstant<decltype(parameters)>(parameters, parameters);

		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
	}

	if (ImGui::Button("\ue145")) {
		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());

		EditorCommandInvoker::Execute(
			std::make_unique<EditorCommandResizeContainer<decltype(parameters)>>(
				parameters,
				static_cast<size_t>(parameters.size() + 1)
			)
		);

		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
	}
	ImGui::SameLine();

	if (ImGui::Button("\ue15b")) {
		if (!parameters.empty()) {
			EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());

			i32 id = static_cast<i32>(parameters.id_by_index(parameters.size() - 1).value());
			EditorValueChangeCommandHandler::GenCommandInstant(parameters, id, std::identity{}, parameters.at_mut_unchecked(id));

			EditorCommandInvoker::Execute(
				std::make_unique<EditorCommandResizeContainer<decltype(parameters)>>(
					parameters,
					static_cast<size_t>(parameters.size() - 1)
				)
			);

			EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
		}
	}

	if (paramSelectedId.has_value() && !parameters.is_valid_id(paramSelectedId.value())) {
		paramSelectedId.reset();
		stringEditBuffer.reset();
	}

	if (ImGui::BeginTable("ParameterTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoHostExtendX)) {
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 150.0f);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);

		i32 offset = 0;
		for (i32 i = 0; i < parameters.size(); ++i) {
			while (!parameters.is_valid_id(i + offset)) {
				++offset;
			}
			i32 id = i + offset;
			Parameter& parameter = parameters.at_mut_unchecked(id);

			ImGui::TableNextRow();

			bool isSelected = paramSelectedId.has_value() && paramSelectedId.value() == id;

			ImGui::TableSetColumnIndex(0);

			if (ImGui::Button(std::format("\ue14d##{}", id).c_str())) {
				// copy name to clipboard
				ImGui::SetClipboardText(parameter.name.c_str());
			}
			ImGui::SameLine();
			if (stringEditBuffer.has_value() && isSelected) {
				ImGui::SetKeyboardFocusHere();
				i32 textFlags =
					ImGuiInputTextFlags_AutoSelectAll |
					ImGuiInputTextFlags_EnterReturnsTrue |
					ImGuiInputTextFlags_EscapeClearsAll;
				ImGui::InputText(std::format("##EditorParamName{}", id).c_str(), &stringEditBuffer.value(), textFlags);
				if (ImGui::IsItemDeactivatedAfterEdit()) {
					EditorValueChangeCommandHandler::GenCommandInstant(parameters, id, &Parameter::name, stringEditBuffer.value());
				}

				if (ImGui::IsItemDeactivated()) {
					stringEditBuffer.reset();
				}
			}
			else {
				if (ImGui::Selectable(std::format("{}##{}", parameter.name, id).c_str(), &isSelected, ImGuiSelectableFlags_AllowDoubleClick)) {
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
						stringEditBuffer = parameter.name;
					}
					else {
						paramSelectedId = id;
					}
				}
			}

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-1.0f);
			std::bitset<2> result = std::visit([id](auto& val) {
				using T = std::decay_t<decltype(val)>;
				return ValueEditor::show_object<T>(std::format("##EditorParamValue{}", id)).show_gui(val);
			}, parameter.value);
			if (result == 0b01) {
				EditorValueChangeCommandHandler::GenCommand(parameters, id, &Parameter::value);
			}
			else if (result == 0b10) {
				EditorValueChangeCommandHandler::End();
			}

			ImGui::TableSetColumnIndex(2);
			i32 typeIndex = static_cast<i32>(parameter.type);
			ImGui::SetNextItemWidth(-1.0f);
			if (ImGui::Combo(std::format("##EditorParamType{}", id).c_str(), &typeIndex, ParameterTypeNames.data(), ParameterTypeCount)) {
				EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());

				EditorValueChangeCommandHandler::GenCommandInstant(parameters, id, &Parameter::type, static_cast<ParameterType>(typeIndex));
				EditorValueChangeCommandHandler::GenCommandInstant(parameters, id, &Parameter::value, ParameterDefaultValues[typeIndex]);

				EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
			}
		}

		ImGui::EndTable();
	}

	ImGui::End();
}

void szg::EditorParameterWindow::save() const {
	szg::JsonAsset jsonAsset{ std::format("[[game]]/{}", filename.value().string()), "param" };

	jsonAsset.get().clear();

	nlohmann::json jsonArray = nlohmann::json::array();
	for (const auto& param : parameters) {
		nlohmann::json jsonParam;
		i32  typeIndex = static_cast<i32>(param.type);

		jsonParam["name"] = param.name;
		jsonParam["type"] = ParameterTypeNames[typeIndex];
		std::visit([&jsonParam](const auto& val) {
			jsonParam["value"] = val;
		}, param.value);
		jsonArray.emplace_back(jsonParam);
	}

	jsonAsset.get() = jsonArray;

	jsonAsset.save();
}

void szg::EditorParameterWindow::load() {
	szg::JsonAsset jsonAsset{ std::format("[[game]]/{}", filename.value().string()), "param" };

	if (jsonAsset.cget().is_null()) {
		return;
	}

	if (!jsonAsset.cget().is_array()) {
		return;
	}

	for (auto& jsonParam : jsonAsset.cget()) {
		if (!jsonParam.contains("name") || !jsonParam["name"].is_string()) {
			continue;
		}
		if (!jsonParam.contains("type") || !jsonParam["type"].is_string()) {
			continue;
		}
		if (!jsonParam.contains("value")) {
			continue;
		}
		std::string name = jsonParam["name"];
		std::string typeName = jsonParam["type"];
		ParameterType type;
		ParameterVariant value;

		for (i32 i = 0; i < ParameterTypeCount; ++i) {
			if (typeName != ParameterTypeNames[i]) {
				continue;
			}

			type = static_cast<ParameterType>(i);
			value = std::visit([&jsonParam](auto&& arg) -> ParameterVariant {
				using T = std::decay_t<decltype(arg)>;
				return jsonParam["value"].get<T>();
			}, ParameterDefaultValues[i]);
			break;
		}

		parameters.emplace_back(name, type, value);
	}
}

#endif // DEBUG_FEATURES_ENABLE
