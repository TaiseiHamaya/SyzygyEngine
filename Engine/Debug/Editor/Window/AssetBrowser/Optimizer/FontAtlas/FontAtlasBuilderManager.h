#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include <Library/Utility/Template/SingletonInterface.h>

#include <Library/Externals/msdf-atlas-gen/msdf-atlas-gen/msdf-atlas-gen.h>

/// <summary>
/// freetype/msdfgen/msdf-atlas-genのハンドラ管理クラス
/// </summary>
namespace szg {

class FontAtlasBuilderManager final : public SingletonInterface<FontAtlasBuilderManager> {
	SZG_CLASS_SINGLETON(FontAtlasBuilderManager)

public:
	static void Initialize();
	static void Finalize();

public:
	static msdfgen::FreetypeHandle* FreetypeHandle();
	static const msdf_atlas::Charset& Charset();

private:
	msdf_atlas::Charset charset;
	msdfgen::FreetypeHandle* freetype;
};

}; // szg

#endif // DEBUG_FEATURES_ENABLE
