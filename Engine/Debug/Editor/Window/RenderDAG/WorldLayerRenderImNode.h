#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "IRenderDagImNode.h"

#include <Library/Math/Vector2.h>

#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/Debug/Editor/Adapter/EditorValueField.h"

namespace szg {

class WorldLayerRenderImNode : public IRenderDagImNode {
public:
	WorldLayerRenderImNode();
	~WorldLayerRenderImNode() override = default;

public:
	struct Viewport {
		EditorValueField<r32> left{ "Left", 0.0f };
		EditorValueField<r32> top{ "Top", 0.0f };
		EditorValueField<r32> width{ "Width", ProjectSettings::ClientSize().x };
		EditorValueField<r32> height{ "Height", ProjectSettings::ClientSize().y };
		EditorValueField<r32> minDepth{ "MinDepth", 0.0f };
		EditorValueField<r32> maxDepth{ "MaxDepth", 1.0f };
	};

	struct Scissor {
		EditorValueField<Vector2> topLeft{ "TopLeft", CVector2::ZERO };
		EditorValueField<Vector2> bottomRight{ "BottomRight", ProjectSettings::ClientSize() };
	};

	struct Data {
		EditorValueField<Vector2> outputSize{ "OutputSize", ProjectSettings::ClientSize() };
		EditorValueField<u32> worldIndex{ "WorldIndex" };
		EditorValueField<u32> layerIndex{ "LayerIndex" };
		EditorValueField<u32> cameraId{ "CameraId" };
		EditorValueField<Vector2> gBufferSize{ "GBufferSize", ProjectSettings::ClientSize() };
		EditorValueField<bool> isClearDepthStencil{ "ClearDepthStencil", true };
		Viewport gBufferViewport;
		Scissor layerScissor;
	};

public:
	void draw() override;

public:
	const Data& get_data() const;
	void set_data(const Data& data_);

private:
	Data data;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
