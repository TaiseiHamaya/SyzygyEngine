#include "LayerRenderSubtree.h"

using namespace szg;

#include "Engine/Module/Render/RenderPipeline/Deferred/Lighting/DirectionalLightingPipeline.h"
#include "Engine/Module/Render/RenderPipeline/Deferred/Lighting/NonLightingPixelPipeline.h"
#include "Engine/Module/Render/RenderPipeline/Deferred/Lighting/PointLightingPipeline.h"
#include "Engine/Module/Render/RenderPipeline/Deferred/Mesh/SkinningMeshDeferredPipeline.h"
#include "Engine/Module/Render/RenderPipeline/Deferred/Mesh/StaticMeshDeferredPipeline.h"
#include "Engine/Module/Render/RenderPipeline/Forward/FontRenderingNode/FontRenderingPipeline.h"
#include "Engine/Module/Render/RenderPipeline/Forward/Primitive/Rect3dPipeline.h"

void LayerRenderSubtree::setup() {
	std::vector<std::shared_ptr<BaseRenderPipeline>> nodes;
	//// WriteGBuffer
	{
		auto node = std::make_shared<StaticMeshDeferredPipeline>();
		node->initialize();
		nodes.emplace_back(node);
	}
	{
		auto node = std::make_shared<SkinningMeshDeferredPipeline>();
		node->initialize();
		nodes.emplace_back(node);
	}
	// ライティングパス
	{
		{

			std::shared_ptr<NonLightingPixelPipeline> nonLightingPixelNode;
			nonLightingPixelNode = std::make_shared<NonLightingPixelPipeline>();
			nonLightingPixelNode->initialize();
			nodes.emplace_back(nonLightingPixelNode);
		}
		{
			std::shared_ptr<DirectionalLightingPipeline> directionalLightingNode;
			directionalLightingNode = std::make_shared<DirectionalLightingPipeline>();
			directionalLightingNode->initialize();
			nodes.emplace_back(directionalLightingNode);
		}
		{
			std::shared_ptr<PointLightingPipeline> pointLightingNode;
			pointLightingNode = std::make_shared<PointLightingPipeline>();
			pointLightingNode->initialize();
			nodes.emplace_back(pointLightingNode);
		}
	}
	// Primitiveパス
	{
		auto node = std::make_shared<Rect3dPipeline>();
		node->initialize();
		nodes.emplace_back(node);
	}
	{
		auto node = std::make_shared<FontRenderingPipeline>();
		node->initialize();
		nodes.emplace_back(node);
	}

	renderSubtree.initialize(std::move(nodes));
}

void LayerRenderSubtree::begin_nodes() {
	renderSubtree.begin();
}

void LayerRenderSubtree::next_node() {
	renderSubtree.next();
}
