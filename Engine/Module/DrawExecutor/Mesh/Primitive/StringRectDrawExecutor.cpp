#include "StringRectDrawExecutor.h"

using namespace szg;

#include "Engine/Assets/Texture/TextureLibrary.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"
#include "Engine/Module/World/Mesh/Primitive/StringRectInstance.h"
#include "Engine/Assets/PrimitiveGeometry/PrimitiveGeometryAsset.h"
#include "Engine/Assets/PrimitiveGeometry/PrimitiveGeometryLibrary.h"

void StringRectDrawExecutor::reinitialize(BlendMode type_, u32 maxRenderingChar_, u32 maxInstance_) {
	type = type_;
	maxInstance = maxRenderingChar_;
	maxStringInstance = maxInstance_;
	instanceCounter = 0;
	strings.initialize(maxStringInstance);
	material.initialize(maxStringInstance);

	chars.initialize(maxInstance);
}

void StringRectDrawExecutor::draw_command() const {
	if (instanceCounter == 0) {
		return;
	}

	auto& commandList = DxCommand::GetCommandList();
	commandList->IASetVertexBuffers(0, 0, nullptr);
	auto geometry = PrimitiveGeometryLibrary::GetPrimitiveGeometry("Rect3D");
	commandList->IASetIndexBuffer(geometry->get_p_ibv());
	commandList->SetGraphicsRootDescriptorTable(0, chars.get_handle_gpu());
	commandList->SetGraphicsRootDescriptorTable(1, strings.get_handle_gpu());
	commandList->SetGraphicsRootDescriptorTable(2, material.get_handle_gpu());

	commandList->DrawIndexedInstanced(6, instanceCounter, 0, 0, 0);
}

void StringRectDrawExecutor::write_to_buffer(Reference<const StringRectInstance> instance) {
	if (!instance || !instance->is_draw()) {
		return;
	}

	if (!instance->glyph_bindless_index().has_value()) {
		return;
	}

	u32 beginIndex;
	u32 stringIndex;
	const std::vector<GlyphRenderingData>& glyphData = instance->glyph_data_imm();
	u32 numChars = static_cast<u32>(glyphData.size());
	if (numChars == 0) {
		return;
	}

	{
		std::lock_guard<std::mutex> lock{ writeBufferMutex };
		if (instanceCounter == 0) {
			stringInstanceCounter = 0;
		}
		// 容量オーバー
		if (stringInstanceCounter >= maxStringInstance) {
			return;
		}
		if (instanceCounter + numChars >= maxInstance) {
			return;
		}

		stringIndex = stringInstanceCounter;
		beginIndex = instanceCounter;

		instanceCounter += numChars;
		++stringInstanceCounter;
	}

	const Affine& local = instance->local_affine();
	const Affine& world = instance->world_affine();
	Affine transformAffine = local * world;
	// StringBuffer書き込み
	StringBuffer stringBuffer{
		.world = transformAffine.to_matrix(),
		.fontSize = instance->font_size(),
		.fontScale = instance->font_scale(),
		.offset = instance->offset_imm(),
		.glyphBindlessIndex = instance->glyph_bindless_index().value(),
	};
	strings[stringIndex] = stringBuffer;

	// MaterialDataBuffer4書き込み
	MaterialDataBufferRGBA materialData{
		.color = instance->get_material().color,
		.lighting = LighingType::None,
		.shininess = 0.0f,
		.textureIndex = instance->font_texture_index(),
		.uvTransformMatrix = CMatrix3x3::IDENTITY,
	};
	material[stringIndex] = materialData;

	// CharBuffer書き込み
	r32 offsetX = 0.0f;
	for (u32 i = 0; i < numChars; ++i) {
		const GlyphRenderingData& glyph = glyphData[i];
		CharBuffer charBuffer{
			.rectOffset = Vector2{ glyph.bottomLeft.x + offsetX, glyph.bottomLeft.y },
			.size = glyph.size,
			.glyphIndex = glyph.glyphIndex,
			.instanceIndex = stringIndex,
		};
		chars[beginIndex + i] = charBuffer;
	}
}
