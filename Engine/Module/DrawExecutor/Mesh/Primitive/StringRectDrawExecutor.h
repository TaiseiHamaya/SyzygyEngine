#pragma once

#include <mutex>

#include "../../BaseDrawExecutor.h"

#include <Library/Utility/Template/Reference.h>

#include "Engine/GraphicsAPI/DirectX/DxResource/StructuredBuffer/StructuredBuffer.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/BufferObjects.h"

namespace szg {

class StringRectInstance;
struct StringData;
struct CharRectData;
class FontAtlasMSDFAsset;

class StringRectDrawExecutor final : public BaseDrawExecutor<StringRectInstance> {
public:
	struct CharBuffer {
		Vector2 rectOffset;
		Vector2 size;
		u32 glyphIndex;
		u32 instanceIndex;
	};

	struct StringBuffer {
		Matrix4x4 world;
		float fontSize;
		float fontScale;
		Vector2 offset;
		u32 glyphBindlessIndex;
	};

public:
	StringRectDrawExecutor() noexcept = default;
	~StringRectDrawExecutor() noexcept = default;

	SZG_CLASS_MOVE_ONLY(StringRectDrawExecutor)

public:
	void reinitialize(BlendMode type_, u32 maxRenderingChar_, u32 maxInstance_);
	void draw_command() const override;
	void write_to_buffer(Reference<const StringRectInstance> instance) override;

private:
	BlendMode type;
	u32 maxStringInstance{ 0 };
	u32 stringInstanceCounter{ 0 };

	StructuredBuffer<StringBuffer> strings;
	StructuredBuffer<MaterialDataBufferRGBA> material;

	StructuredBuffer<CharBuffer> chars;

	std::mutex writeBufferMutex;
};

}; // szg
