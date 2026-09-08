#include <array>
#include <optional>

#include <imgui.h>

namespace szg {

namespace EditorUtils {

template<typename T, size_t ItemCount>
inline std::optional<T> DrawRadioButton(const T& current, const std::array<string_literal, ItemCount>& labels, std::string_view extensionId = "") {
	std::optional<T> result = std::nullopt;
	for (i32 i = 0; i < static_cast<i32>(ItemCount); ++i) {
		const ImVec2 textSize = ImGui::CalcTextSize(labels[i]);
		const r32 radioWidth = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemInnerSpacing.x + textSize.x;
		if (i > 0 && radioWidth > ImGui::GetContentRegionAvail().x) {
			ImGui::NewLine();
		}
		if (ImGui::RadioButton(std::format("{}##{}", labels[i], extensionId).c_str(), current == static_cast<T>(i))) {
			result = static_cast<T>(i);
		}
		ImGui::SameLine();
	}
	ImGui::NewLine();

	return result;
}

} // namespace EditorUtils

} // namespace szg
