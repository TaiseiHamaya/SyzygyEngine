#include "RenderPath.h"

#include "Engine/Application/Logger.h"
#include "Engine/Module/Render/RenderPipeline/BaseRenderPipeline.h"

using namespace szg;

void RenderPath::initialize(std::vector<std::shared_ptr<BaseRenderPipeline>>&& list) {
	// サイズ0のPathはバグるので止める
	szgErrorIf(list.empty(), "The array size of RenderPath must be at least 1.");
	renderNodeList = std::move(list);
}

void RenderPath::initialize(std::initializer_list<std::shared_ptr<BaseRenderPipeline>>&& list) {
	// サイズ0のPathはバグるので止める
	szgErrorIf(list.size() == 0, "The array size of RenderPath must be at least 1.");
	// 全て転送
	renderNodeList = std::move(list);
}

void RenderPath::use() {
	// nowNodeのリセットしておく
	nowNode = renderNodeList.end();
	//for (auto& renderNode : renderNodeList) {
	//	renderNode->use();
	//}
}

bool RenderPath::begin() {
	// nowNodeのリセット
	nowNode = renderNodeList.begin();
	return next();
}

bool RenderPath::next() {
	// 末尾に行っていなければ次の処理を開始
	if (nowNode != renderNodeList.end()) {
		(*nowNode)->stack_pso_command();
		// 次に進める
		++nowNode;
		return true;
	}
	else {
		// 終わっていれば処理を終了する
		return false;
	}
}

bool RenderPath::is_end() {
	return nowNode == renderNodeList.end();
}
