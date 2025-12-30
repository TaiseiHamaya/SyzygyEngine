#include "WorldLayerRenderNode.h"

using namespace szg;

#include "../WorldRenderCollection.h"
#include "Engine/Application/Logger.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/RenderingSystemValues.h"
#include "Engine/Module/World/Camera/CameraInstance.h"

void WorldLayerRenderNode::setup(Data&& data_) {
	data = std::move(data_);
	subtree.setup(data.gBuffer.texture);
}

void WorldLayerRenderNode::stack_command() {
	// カメラ取得
	Reference<CameraInstance> camera = data.layerData.worldRenderCollection->camera_at(data.layerData.cameraId);
	if (!camera) {
		szgWarning("Camera is invalid. LayerIndex: {}", data.layerData.index);
		return;
	}
	if (!camera->is_active()) {
		return;
	}

	camera->transfer();

	auto&& commandList = DxCommand::GetCommandList();
	Reference<DepthStencilTexture> depthStencilTexture = RenderingSystemValues::GetDepthStencilTexture();

	// RenderTargetの設定
	// ViewPortの設定
	commandList->RSSetViewports(1, &data.gBuffer.viewport);
	// シザー矩形の設定
	commandList->RSSetScissorRects(1, &data.gBuffer.rect);
	data.gBuffer.renderTarget->begin_write(true, depthStencilTexture);
	depthStencilTexture->start_write();
	if (data.layerData.isClearDepthStencil) {
		depthStencilTexture->get_as_dsv()->clear();
	}

	// ----- GBufferPass -----
	// StaticMesh
	subtree.begin_nodes();
	camera->register_world_projection(2);
	data.layerData.worldRenderCollection->staticMeshDrawManager.draw_layer(data.layerData.index);

	// SkinningMesh
	subtree.next_node();
	camera->register_world_projection(2);
	data.layerData.worldRenderCollection->skinningMeshDrawManager.draw_layer(data.layerData.index);

	// ----- LightingPass -----
	// ViewPortの設定
	commandList->RSSetViewports(1, &data.layerData.viewport);
	// シザー矩形の設定
	commandList->RSSetScissorRects(1, &data.layerData.rect);
	data.outputRenderTargetGroup->begin_write(data.layerData.isClearRenderTarget, depthStencilTexture);

	data.gBuffer.texture[0]->start_read();
	data.gBuffer.texture[1]->start_read();
	depthStencilTexture->start_read();

	// NonLightingPixel
	subtree.next_node(); // 自動実行

	// DirectionalLighting
	subtree.next_node();
	camera->register_world_lighting(1);
	data.layerData.worldRenderCollection->directionalLightingExecutors[data.layerData.index].draw_command();

	// PointLighting
	subtree.next_node();
	camera->register_world_projection(1);
	camera->register_world_lighting(6);
	data.layerData.outputTextureSize.stack_command(7);
	data.layerData.worldRenderCollection->pointLightingExecutors[data.layerData.index].draw_command();

	// ----- PrimitivePass -----
	// Rect
	subtree.next_node();
	depthStencilTexture->start_write();
	data.outputRenderTargetGroup->begin_write(false, depthStencilTexture);
	camera->register_world_projection(3);
	camera->register_world_lighting(4);
	data.layerData.worldRenderCollection->directionalLightingExecutors[data.layerData.index].set_command(5);
	data.layerData.worldRenderCollection->rect3dDrawManager.draw_layer(data.layerData.index);

	// StringRect
	subtree.next_node();
	camera->register_world_projection(3);
	data.layerData.worldRenderCollection->stringRectDrawManager.draw_layer(data.layerData.index);

	subtree.next_node();
}

const WorldLayerRenderNode::Data& WorldLayerRenderNode::data_imm() const {
	return data;
}
