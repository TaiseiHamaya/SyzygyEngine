#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "IRenderDagImNode.h"

#include "Engine/Debug/Editor/Adapter/EditorValueField.h"

#include <string>

namespace szg {

class StaticTextureImNode : public IRenderDagImNode {
public:
	StaticTextureImNode();
	~StaticTextureImNode() override = default;

public:
	struct Data {
		EditorValueField<std::string> textureFilename{ "Filename" };
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
