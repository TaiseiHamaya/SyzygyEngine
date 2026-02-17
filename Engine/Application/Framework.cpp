#include "Framework.h"

#include "Engine/Application/WinApp.h"

using namespace szg;

void Framework::run() {
	WinApp::Initialize();

	initialize();

	WinApp::ShowAppWindow();

	while (true) {
		WinApp::ProcessMessage();

		if (WinApp::IsEndApp()) {
			break;
		}

		WinApp::BeginFrame();

		WinApp::Update();

		WinApp::Draw();

		WinApp::EndFrame();
	}

	WinApp::Finalize();
}
