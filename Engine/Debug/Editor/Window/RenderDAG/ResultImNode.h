#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "IRenderDagImNode.h"

namespace szg {

class ResultImNode final : public IRenderDagImNode {
public:
	ResultImNode();
	~ResultImNode() override = default;

public:
	void draw() override;

private:
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
