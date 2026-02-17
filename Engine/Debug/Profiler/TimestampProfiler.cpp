#ifdef DEBUG_FEATURES_ENABLE

#include "TimestampProfiler.h"

#include <format>

#include <imgui.h>

using namespace szg;

void TimestampProfiler::debug_gui() {
	auto flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoHostExtendX;
	if (ImGui::BeginTable("Timestamps", 2, flags)) {
		ImGui::TableSetupColumn("Timestamps", ImGuiTableColumnFlags_WidthFixed, 100);
		ImGui::TableSetupColumn("Duration", ImGuiTableColumnFlags_WidthFixed, 60);
		ImGui::TableHeadersRow();

		for (auto& timestamp : timestamps) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(timestamp.name.c_str());
			ImGui::TableSetColumnIndex(1);
			if (timestamp.duration) {
				ImGui::Text(std::format("{: >9}", timestamp.duration.value()).c_str());
			}
			else {
				ImGui::Text("      N/A");
			}
		}

		ImGui::EndTable();
	}
}

void TimestampProfiler::clear_timestamps() {
	timestamps.clear();
}

void TimestampProfiler::timestamp(const std::string& name) {
	auto now = clock::now();

	if (!timestamps.empty()) {
		auto prev = timestamps.rbegin();
		prev->duration = std::chrono::duration_cast<std::chrono::microseconds>(now - prev->time);
	}

	timestamps.emplace_back(name, now, std::nullopt);
}

#endif // _DEBUG
