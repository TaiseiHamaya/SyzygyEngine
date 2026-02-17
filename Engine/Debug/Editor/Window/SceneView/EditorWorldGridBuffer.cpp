#include "EditorWorldGridBuffer.h"

#include <Library/Utility/Tools/MathEPS.h>

#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"

namespace {

/// <summary>
/// 20進数での桁数を取得
/// </summary>
/// <param name="x"></param>
/// <returns></returns>
u64 Base20Digits(r32 x) {
	if (x <= 1.0) return 0;
	// log20(x) = log2(x) / log2(20)
	return static_cast<u64>(std::ceilf(std::log2f(x) / std::log2f(20)));
}

} // namespace

void szg::EditorWorldGridBuffer::transfer(const Vector3& centerBase, r32 cameraY) {
	u64 digits20 = Base20Digits(std::abs(cameraY));
	// 10^(digits-1)をする (0の場合は0にする)
	u64 exponent = digits20 == 0 ? 0 : digits20 - 1;
	u64 scale = eps::pow(10, exponent);

	Vector2 fixedCenter;
	// scaleの倍数に丸め込む
	fixedCenter.x = std::floor(centerBase.x / scale) * scale;
	fixedCenter.y = std::floor(centerBase.z / scale) * scale;
	
	data.data_mut()->center = fixedCenter;
	data.data_mut()->scale = static_cast<r32>(scale);
}

void szg::EditorWorldGridBuffer::stack_command() const {
	auto& commandList = DxCommand::GetCommandList();
	commandList->SetGraphicsRootConstantBufferView(0, data.get_resource()->GetGPUVirtualAddress());

	static const u32 GRID_SIZE = 20;
	static const u32 NUM_LINES = GRID_SIZE + 1;
	static const u32 INSTANCES = NUM_LINES * 2;
	static const u32 TOTAL_VERTICES = INSTANCES * 2;

	commandList->DrawInstanced(2, INSTANCES, 0, 0);
}
