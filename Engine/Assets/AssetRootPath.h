#pragma once

#include <array>

namespace szg {

enum class AssetRootType {
	Unselect,
	Engine,
	Game,
	Editor,

	Max,
};

static constexpr i32 ASSET_ROOT_TYPE_MAX = static_cast<i32>(AssetRootType::Max);

static constexpr std::array<string_literal, ASSET_ROOT_TYPE_MAX>  ROOT_TAG{
	"[[unselect]]",
	"[[szg]]",
	"[[game]]",
	"[[editor]]",
};

static constexpr std::array<wstring_literal, ASSET_ROOT_TYPE_MAX>  ROOT_TAG_W{
	L"[[unselect]]",
	L"[[szg]]",
	L"[[game]]",
	L"[[editor]]",
};

static constexpr std::array<string_literal, ASSET_ROOT_TYPE_MAX>  ROOT_PATH{
	"",
	".\\SyzygyEngine\\EngineResources",
	".\\Game\\Assets",
	".\\SyzygyEngine\\EditorResources",
};

} // namespace szg
