#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "IRenderDagImNode.h"

#include <Library/Math/Vector2.h>

#include "Engine/Application/ProjectSettings/ProjectSettings.h"
#include "Engine/Debug/Editor/Adapter/EditorValueField.h"
#include "Engine/Loader/RenderPath/RenderNodeType.h"

#include <string>
#include <vector>

namespace szg {

class PostEffectImNode : public IRenderDagImNode {
public:
	PostEffectImNode();
	~PostEffectImNode() override = default;

public:
	struct Data {
		EditorValueField<u32> peType{ "Type" };
		EditorValueField<Vector2> outputSize{ "OutputSize", ProjectSettings::ClientSize() };
		EditorValueField<bool> isUseRuntime{ "UseRuntime", false };
		EditorValueField<std::string> effectTagName{ "EffectTag", "" };
		std::vector<u64> extraInputs;
	};

public:
	void draw() override;

	void remove_extra_input_pins();
	void update_extra_input_pins();

public:
	const Data& get_data() const;
	void set_data(const Data& data_);

	PostEffectType post_effect_type() const;

private:
	Data data;

private:
	static constexpr std::array<string_literal, static_cast<i32>(PostEffectType::Max)> postEffectName{
		"Unknown",
		"ブルーム",
		"色収差",
		"ガウシアンブラー",
		"グレースケール",
		"輝度抽出",
		"テクスチャ合成4",
		"Outline",
		"ラジアルブラー",
	};
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
