#include "DxDevice.h"

#include "Engine/Application/Logger.h"

using namespace szg;

DxDevice& DxDevice::GetInstance() noexcept {
	static DxDevice instance{ };
	return instance;
}

void DxDevice::Initialize() {
	DxDevice& instance = GetInstance();
	// DxgiFactory生成
	instance.create_dxgiFactory();
	// Adapter探しの旅
	instance.create_adapter();
	// Device生成
	instance.create_device();
	// 機能チェック
	instance.check_future();

	szgInformation("Complete create D3D12Device");
}

void DxDevice::create_dxgiFactory() {
	// ----------DXGIファクトリーの生成----------
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	szgCriticalIf(FAILED(hr), "Failed to create DXGI factory.");
}

void DxDevice::create_adapter() {
	// ----------使用するアダプタを検索----------
// 良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
		DXGI_ADAPTER_DESC3 adapterDesc{};
		HRESULT hr = useAdapter->GetDesc3(&adapterDesc);
		szgCriticalIf(FAILED(hr), "Failed function call. \'IDXGIAdapter4::GetDesc3\'");
		// ソフトウェアアダプターでなければ採用する
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// ログ出力
			szgInformation(L"Use Adapter : {}", adapterDesc.Description);
			break;
		}
		// ソフトウェアアダプタの場合は無視
		useAdapter = nullptr;
	}
	// 見つからなかったら起動しない
	szgCriticalIf(useAdapter == nullptr, "Failed initialize GPU adapter.");
}

void DxDevice::create_device() {
	// ----------デバイスの生成----------
// 機能レベル一覧
	D3D_FEATURE_LEVEL featureLevel[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	string_literal featureLevelString[] = { "12.2","12.1" ,"12.0" };
	// 高い順に試す
	for (size_t i = 0; i < _countof(featureLevel); ++i) {
		// デバイス生成を検証
		HRESULT hr = D3D12CreateDevice(useAdapter.Get(), featureLevel[i], IID_PPV_ARGS(&device));
		if (SUCCEEDED(hr)) {
			// device生成完了通知
			szgInformation("FeatureLevel : {}", featureLevelString[i]);
			break;
		}
	}
	// 失敗したら停止
	szgCriticalIf(device == nullptr, "DirectX device creation is failed.");
}

void DxDevice::check_future() {
	HRESULT hr;
	{ // ShaderModel Support
		D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_6 };
		hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
		if (FAILED(hr) || shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_6) {
			szgCritical("Your hardware isn't support ShaderModel6.6.");
		}
	}

	{ // DXGI Format Support
		D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupportRGB10UNORM = { DXGI_FORMAT_R10G10B10A2_UNORM, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE };
		hr = device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupportRGB10UNORM, sizeof(formatSupportRGB10UNORM));
		if (FAILED(hr) || !(formatSupportRGB10UNORM.Support1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET)) {
			szgCritical("Your hardware isn't support.");
		}
		D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupportR32UINT = { DXGI_FORMAT_R32_UINT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE };
		hr = device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupportR32UINT, sizeof(formatSupportR32UINT));
		if (FAILED(hr) || 
			!(formatSupportR32UINT.Support1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET) || 
			!(formatSupportR32UINT.Support1 & D3D12_FORMAT_SUPPORT1_SHADER_LOAD)
			) {
			szgCritical("Your hardware isn't support.");
		}
	}

	{ // Bindless Resource Support
		D3D12_FEATURE_DATA_D3D12_OPTIONS featureSupport{};
		hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &featureSupport, sizeof(featureSupport));
		if (FAILED(hr) || featureSupport.ResourceBindingTier == 0) {
			szgCritical("Your hardware isn't support.");
		}
	}

	{ // MeshShader Support
		D3D12_FEATURE_DATA_D3D12_OPTIONS7 featureData = {};
		hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &featureData, sizeof(featureData));
		if (FAILED(hr) || featureData.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED) {
			szgCritical("Your hardware isn't support.");
		}
	}
}
