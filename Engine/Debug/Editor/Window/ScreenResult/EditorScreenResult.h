#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IEditorWindow.h"

#include "Engine/GraphicsAPI/DirectX/DxResource/TextureResource/TempTexture.h"

namespace szg {

class EditorScreenResult final : public IEditorWindow {
public:
	void initialize();

	void draw() override;

private:
	void copy_screen_result();

	void set_imgui_command();

private:
	TempTexture screenResultCpy;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
