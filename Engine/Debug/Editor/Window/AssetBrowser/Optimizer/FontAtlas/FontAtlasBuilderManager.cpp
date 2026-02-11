#ifdef DEBUG_FEATURES_ENABLE

#include "FontAtlasBuilderManager.h"

using namespace szg;

void FontAtlasBuilderManager::Initialize() {
	auto& instance = GetInstance();
	if (instance.freetype) {
		return;
	}
	instance.freetype = msdfgen::initializeFreetype();
	instance.charset.load("./SyzygyEngine/EditorResources/charset.txt");
}

void FontAtlasBuilderManager::Finalize() {
	auto& instance = GetInstance();
	deinitializeFreetype(instance.freetype);
}

msdfgen::FreetypeHandle* FontAtlasBuilderManager::FreetypeHandle() {
	auto& instance = GetInstance();
	return instance.freetype;
}

const msdf_atlas::Charset& FontAtlasBuilderManager::Charset() {
	auto& instance = GetInstance();
	return instance.charset;
}

#endif // DEBUG_FEATURES_ENABLE
