#ifdef DEBUG_FEATURES_ENABLE

#include "EditorScreenResult.h"

using namespace szg;

#include <imgui.h>

#include "Engine/GraphicsAPI/DirectX/DxSwapChain/DxSwapChain.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/ScreenTexture.h"

void EditorScreenResult::initialize(bool isActive_) {
	isActive = isActive_;

	screenResultCpy.initialize();
}

void EditorScreenResult::draw() {
	copy_screen_result();
	set_imgui_command();
}

void EditorScreenResult::copy_screen_result() {
	auto& command = DxCommand::GetCommandList();
	Reference<ScreenTexture> screen = DxSwapChain::GetWriteBufferTexture();

	screenResultCpy.start_copy_dest();
	screen->start_read();

	command->CopyResource(screenResultCpy.get_resource().Get(), screen->get_resource().Get());
	screenResultCpy.start_read();
}

void EditorScreenResult::set_imgui_command() {
	if (!isActive) {
		return;
	}

	ImGui::Begin("SceneView", &isActive, ImGuiWindowFlags_NoScrollbar);

	update_focus();

	// Imageの位置とサイズを計算
	ImVec2 winSize = ImGui::GetContentRegionAvail();
	ImVec2 winPos = ImGui::GetWindowPos();

	float aspectX = winSize.x / 16;
	float aspectY = winSize.y / 9;

	ImVec2 imgSize = aspectX > aspectY ?
		ImVec2{ winSize.y / 9 * 16, winSize.y } :
		ImVec2{ winSize.x, winSize.x / 16 * 9 };

	ImVec2 cursorPos = {
		(winSize.x - imgSize.x) * 0.5f + winPos.x + ImGui::GetCursorPosX(),
		(winSize.y - imgSize.y) * 0.5f + winPos.y + ImGui::GetCursorPosY()
	};
	ImGui::SetCursorScreenPos(cursorPos);

	ImGui::Image(screenResultCpy.get_as_srv()->handle().ptr, imgSize);

	ImGui::End();
}

#endif // DEBUG_FEATURES_ENABLE
