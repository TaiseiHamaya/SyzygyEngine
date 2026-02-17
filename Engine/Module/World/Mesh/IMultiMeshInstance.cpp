#include "IMultiMeshInstance.h"

#include "Engine/Application/Logger.h"
#include "Engine/Assets/Texture/TextureLibrary.h"

using namespace szg;

IMultiMeshInstance::IMultiMeshInstance() noexcept :
	IDrawInstance() {
	materials.clear();
}

IMultiMeshInstance::~IMultiMeshInstance() noexcept = default;

std::vector<IMultiMeshInstance::Material>& IMultiMeshInstance::get_materials() {
	return materials;
}

const std::vector<IMultiMeshInstance::Material>& IMultiMeshInstance::get_materials() const {
	return materials;
}

void IMultiMeshInstance::set_texture(const std::string& name, i32 index) {
	szgWarningIf(index >= materials.size(), "Index({}) is out of range for material.", index);
	materials[index].texture = TextureLibrary::GetTexture(name);
}
