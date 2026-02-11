#pragma once

#include "./IRenderNode.h"

#include <array>

#include <Library/Utility/Template/Reference.h>

#include "Engine/GraphicsAPI/DirectX/DxResource/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Module/Render/RenderPipeline/Deferred/DeferredAdaptor.h"
#include "Engine/Module/Render/RenderSubtree/LayerRenderSubtree.h"
#include "Engine/Module/Render/RenderTargetGroup/BaseRenderTargetGroup.h"

namespace szg {

class CameraInstance;
class WorldRenderCollection;

class WorldLayerRenderNode final : public IRenderNode {
public:
	struct GBufferData {
		std::array<Reference<RenderTexture>, DeferredAdaptor::NUM_GBUFFER> texture;
		Reference<DeferredAdaptor::GBuffersType> renderTarget;
		RECT rect;
		D3D12_VIEWPORT viewport;
	};

	struct LayerData {
		Reference<const WorldRenderCollection> worldRenderCollection;
		u8 index;
		u32 cameraId;

		RECT rect;
		D3D12_VIEWPORT viewport;

		ConstantBuffer<Vector2> outputTextureSize;

		bool isClearRenderTarget;
		bool isClearDepthStencil;
	};

	struct Data {
		GBufferData gBuffer;

		LayerData layerData;

		Reference<BaseRenderTargetGroup> outputRenderTargetGroup;
	};

public:
	WorldLayerRenderNode() = default;
	~WorldLayerRenderNode() noexcept = default;

	SZG_CLASS_MOVE_ONLY(WorldLayerRenderNode)

public:
	void setup(Data&& data_);

	void stack_command() override;

private:
	void execute_gbuffer_pass();
	void execute_lighting_pass();
	void execute_forward_pass();

public:
	const Data& data_imm() const;

private:
	LayerRenderSubtree subtree;

	Data data;
};

}; // szg
