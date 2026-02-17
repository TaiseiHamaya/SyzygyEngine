#include "DxcManager.h"

#include "Engine/Application/Logger.h"

#pragma comment(lib, "dxcompiler.lib")

using namespace szg;

void DxcManager::Initialize() {
	GetInstance().initialize();
}

void DxcManager::initialize() {
	HRESULT hr;
	// ----------コンパイラの生成----------
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils.GetAddressOf()));
	szgCriticalIf(FAILED(hr), "Failed to initialize Shader compiler.");
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler.GetAddressOf()));
	szgCriticalIf(FAILED(hr), "Failed to initialize Shader compiler.");
	hr = dxcUtils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf());
	szgCriticalIf(FAILED(hr), "Failed to initialize Shader compiler.");
}
