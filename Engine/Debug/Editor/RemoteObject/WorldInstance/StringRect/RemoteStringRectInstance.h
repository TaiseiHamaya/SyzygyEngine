#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IRemoteInstance.h"

#include <string>

#include <Library/Math/ColorRGBA.h>
#include <Library/Math/Vector2.h>

#include "Engine/Module/World/Mesh/Primitive/StringRectInstance.h"

namespace szg {

class RemoteStringRectInstance final : public IRemoteInstance<StringRectInstance, StringRectInstance> {
public:
	friend class EditorSceneSerializer;

public:
	RemoteStringRectInstance() = default;
	~RemoteStringRectInstance() = default;

	SZG_CLASS_DEFAULT(RemoteStringRectInstance)

public:
	void setup() override;

	void update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) override;

	void draw_inspector() override;

	nlohmann::json serialize() const override;

	void on_spawn() override;

	void on_destroy() override;

	constexpr InstanceType instance_type() const { return InstanceType::StringRectInstance; }

private:
	EditorValueField<bool> isDraw{ "IsDraw", true };
	EditorValueField<u32> layer{ "Layer", 0 };

	std::string font;
	EditorValueField<r32> fontSize{ "Size", 16.0f };
	EditorValueField<Vector2> pivot{ "Pivot", CVector2::ZERO };
	EditorValueField<std::string> text{ "Text", "Sample Text" };
	EditorValueField<ColorRGBA> color{ "Color", CColorRGBA::WHITE };

	bool isChangedValue{ false };
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
