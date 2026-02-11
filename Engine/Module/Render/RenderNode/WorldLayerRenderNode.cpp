#include "WorldLayerRenderNode.h"

using namespace szg;

#include "../WorldRenderCollection.h"
#include "Engine/Application/Logger.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/RenderingSystemValues.h"
#include "Engine/Module/World/Camera/CameraInstance.h"

void WorldLayerRenderNode::setup(Data&& data_) {
	data = std::move(data_);
	subtree.setup();
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

	execute_gbuffer_pass();

	execute_lighting_pass();

	execute_forward_pass();

	subtree.next_node();
}

void szg::WorldLayerRenderNode::execute_gbuffer_pass() {
	auto&& commandList = DxCommand::GetCommandList();
	Reference<CameraInstance> camera = data.layerData.worldRenderCollection->camera_at(data.layerData.cameraId);
	Reference<DepthStencilTexture> depthStencilTexture = RenderingSystemValues::GetDepthStencilTexture();

	// ----- GBufferPass -----
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
	// StaticMesh
	subtree.begin_nodes();
	camera->register_world_projection(2);
	data.layerData.worldRenderCollection->staticMeshDrawManager.draw_layer(data.layerData.index);

	// SkinningMesh
	subtree.next_node();
	camera->register_world_projection(2);
	data.layerData.worldRenderCollection->skinningMeshDrawManager.draw_layer(data.layerData.index);
}

void szg::WorldLayerRenderNode::execute_lighting_pass() {
	auto&& commandList = DxCommand::GetCommandList();
	Reference<CameraInstance> camera = data.layerData.worldRenderCollection->camera_at(data.layerData.cameraId);
	Reference<DepthStencilTexture> depthStencilTexture = RenderingSystemValues::GetDepthStencilTexture();

	// ----- LightingPass -----
	// 各種テクスチャのread
	for (i32 i = 0; i < DeferredAdaptor::NUM_GBUFFER; ++i) {
		data.gBuffer.texture[i]->start_read();
	}
	depthStencilTexture->start_read();
	// ViewPortの設定
	commandList->RSSetViewports(1, &data.layerData.viewport);
	// シザー矩形の設定
	commandList->RSSetScissorRects(1, &data.layerData.rect);
	data.outputRenderTargetGroup->begin_write(data.layerData.isClearRenderTarget, depthStencilTexture);

	// NonLightingPixel
	subtree.next_node();
	data.gBuffer.texture[0]->get_as_srv()->use(0);
	commandList->DrawInstanced(3, 1, 0, 0);

	// DirectionalLighting
	subtree.next_node();
	for (u32 i = 0; i < DeferredAdaptor::NUM_GBUFFER; ++i) {
		data.gBuffer.texture[i]->get_as_srv()->use(i + 2);
	}
	depthStencilTexture->get_as_srv()->use(4);
	camera->register_world_lighting(1);
	data.layerData.worldRenderCollection->directionalLightingExecutors[data.layerData.index].draw_command();

	// PointLighting
	subtree.next_node();
	for (u32 i = 0; i < DeferredAdaptor::NUM_GBUFFER; ++i) {
		data.gBuffer.texture[i]->get_as_srv()->use(i + 2);
	}
	depthStencilTexture->get_as_srv()->use(4);
	camera->register_world_projection(1);
	camera->register_world_lighting(6);
	data.layerData.outputTextureSize.stack_command(7);
	data.layerData.worldRenderCollection->pointLightingExecutors[data.layerData.index].draw_command();
}

void szg::WorldLayerRenderNode::execute_forward_pass() {
	Reference<CameraInstance> camera = data.layerData.worldRenderCollection->camera_at(data.layerData.cameraId);
	Reference<DepthStencilTexture> depthStencilTexture = RenderingSystemValues::GetDepthStencilTexture();

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
}

const WorldLayerRenderNode::Data& WorldLayerRenderNode::data_imm() const {
	return data;
}
