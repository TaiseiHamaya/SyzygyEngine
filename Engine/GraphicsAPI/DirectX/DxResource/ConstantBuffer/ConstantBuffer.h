#pragma once

#include <format>
#include <typeinfo>

#include <Library/Utility/Template/Reference.h>
#include <Library/Utility/Tools/ConvertString.h>

#include "../ConceptCPUBuffer.h"
#include "Engine/GraphicsAPI/DirectX/DxCommand/DxCommand.h"
#include "Engine/GraphicsAPI/DirectX/DxResource/DxResource.h"

namespace szg {

template<ConceptCPUBufferACE T>
class ConstantBuffer : public DxResource {
public:
	ConstantBuffer() noexcept(false);
	ConstantBuffer(const T& data_) noexcept(false);

	virtual ~ConstantBuffer() noexcept;

	ConstantBuffer(ConstantBuffer&&) = default;
	ConstantBuffer& operator=(ConstantBuffer&&) = default;

public:
	operator T& ();

public:
	Reference<const T> data_imm() const noexcept;
	Reference<T> data_mut() noexcept;

	void unmap();

	void stack_command(u32 index) const;

protected:
	Reference<T> data;
	UINT memorySize;
};

template<ConceptCPUBufferACE T>
inline ConstantBuffer<T>::ConstantBuffer() noexcept(false) {
	memorySize = sizeof(T);
	resource = CreateBufferResource(memorySize);
	T* temp;
	resource->Map(0, nullptr, reinterpret_cast<void**>(&temp));
	data = temp;
	*data = T{};
	std::wstring typeName = ConvertString(typeid(T).name());
	if constexpr (std::is_class_v<T> || std::is_enum_v<T>) {
		typeName = (std::find(typeName.begin(), typeName.end(), ' ') + 1)._Ptr;
	}
	resource->SetName(std::format(L"ConstantBuffer-{}", typeName).c_str());
}

template<ConceptCPUBufferACE T>
inline ConstantBuffer<T>::ConstantBuffer(const T& data_) noexcept(false) : ConstantBuffer<T>() {
	*data = data_;
}

template<ConceptCPUBufferACE T>
inline ConstantBuffer<T>::~ConstantBuffer() noexcept {
	unmap();
}

template<ConceptCPUBufferACE T>
inline ConstantBuffer<T>::operator T& () {
	return *data;
}

template<ConceptCPUBufferACE T>
inline Reference<const T> ConstantBuffer<T>::data_imm() const noexcept {
	return data;
}

template<ConceptCPUBufferACE T>
inline Reference<T> ConstantBuffer<T>::data_mut() noexcept {
	return data;
}

template<ConceptCPUBufferACE T>
inline void ConstantBuffer<T>::unmap() {
	if (resource) {
		resource->Unmap(0, nullptr);
	}
	data.reset();
}

template<ConceptCPUBufferACE T>
inline void ConstantBuffer<T>::stack_command(u32 index) const {
	DxCommand::GetCommandList()->SetGraphicsRootConstantBufferView(index, resource->GetGPUVirtualAddress());
}

}; // szg
