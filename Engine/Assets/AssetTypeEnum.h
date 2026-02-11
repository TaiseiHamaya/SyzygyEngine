#pragma once

#include <array>

namespace szg {

enum class AssetType {
	Unknown,
	Texture,
	Mesh,
	Skeleton,
	Animation,
	Font,
	Json,
	Audio,
	Shader,

	Max,
};

constexpr i32 ASSET_TYPE_MAX = static_cast<i32>(AssetType::Max);

constexpr std::array<string_literal, ASSET_TYPE_MAX> ASSET_TYPE_NAME = {
	"Unknown",
	"Texture",
	"Mesh",
	"Skeleton",
	"Animation",
	"Font",
	"Json",
	"Audio",
	"Shader",
};

}
