#pragma once

#ifdef DEBUG_FEATURES_ENABLE

#include "../IRemoteObject.h"

#include <memory>
#include <vector>

#include <imgui.h>

#include <Library/Math/Affine.h>
#include <Library/Math/Transform3D.h>
#include <Library/Utility/Template/Reference.h>

#include "../../Core/EditorDandDManager.h"
#include "Engine/Debug/Editor/Command/EditorCommandInvoker.h"
#include "Engine/Debug/Editor/Command/EditorSelectCommand.h"

#define TRANSFORM3D_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

namespace szg {

template<typename RuntimeType, typename DebugVisualType = void*>
class IRemoteInstance : public IRemoteObject {
public:
	IRemoteInstance() = default;
	virtual ~IRemoteInstance() = default;

	SZG_CLASS_MOVE_ONLY(IRemoteInstance)

public:
	using RemoteInstanceType = IRemoteInstance<RuntimeType, DebugVisualType>;

public:
	virtual void setup() override;

	virtual void update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) override;

	void draw_hierarchy(Reference<const EditorSelectObject> select) override;

	std::unique_ptr<IRemoteObject> move_force(Reference<const IRemoteObject> child) override;

	void reparent(Reference<IRemoteObject> remoteObject) override;

	void add_child(std::unique_ptr<IRemoteObject> child) override;

	void on_spawn() override;

	void on_destroy() override;

protected:
	Reference<RuntimeType> self;

	std::vector<std::unique_ptr<IRemoteObject>> children;

	std::unique_ptr<DebugVisualType> debugVisual;

	EditorValueField<bool> isUseRuntime{ "Use runtime", false };
	EditorValueField<Transform3D> transform{ "Local transform" };
	Affine worldAffine;
};

template<typename RuntimeType, typename DebugVisualType>
inline void IRemoteInstance<RuntimeType, DebugVisualType>::setup() {
	for (auto& child : children) {
		child->setup();
	}
}

template<typename RuntimeType, typename DebugVisualType>
inline void IRemoteInstance<RuntimeType, DebugVisualType>::update_preview(Reference<RemoteWorldObject> world, Reference<Affine> parentAffine) {
	// 行列計算
	worldAffine = Affine::FromTransform3D(transform.value_imm());
	if (parentAffine) {
		worldAffine *= *parentAffine;
	}
	// 再帰実行
	for (auto& child : children) {
		child->update_preview(world, worldAffine);
	}
}

template<typename RuntimeType, typename DebugVisualType>
inline void IRemoteInstance<RuntimeType, DebugVisualType>::draw_hierarchy(Reference<const EditorSelectObject> select) {
	bool isSelected = select->is_selected(this);
	int flags =
		ImGuiTreeNodeFlags_DrawLinesToNodes |
		ImGuiTreeNodeFlags_FramePadding |
		ImGuiTreeNodeFlags_SpanAllColumns |
		ImGuiTreeNodeFlags_OpenOnArrow; // 矢印で開く
	if (isSelected) {
		flags |= ImGuiTreeNodeFlags_Selected; // 選択時は選択状態にする
	}
	if (isOpen) {
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}
	if (children.empty()) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	}
	isOpen = ImGui::TreeNodeEx(std::format("{}##{}", hierarchyName.value_mut(), (void*)this).c_str(), flags);
	EditorDandDManager::CheckDandDHierarchy(this, parent);

	// こうすると選択できるらしい
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen() && !isSelected) {
		EditorCommandInvoker::Execute(
			std::make_unique<EditorSelectCommand>(this, EditorSelectObjectBody::TransformData{ transform.value_mut(), worldAffine })
		);
	}

	if (isOpen) {
		for (auto& child : children) {
			child->draw_hierarchy(select);
		}
		ImGui::TreePop();
	}
}

template<typename RuntimeType, typename DebugVisualType>
std::unique_ptr<IRemoteObject> IRemoteInstance<RuntimeType, DebugVisualType>::move_force(Reference<const IRemoteObject> child) {
	auto itr = std::find_if(children.begin(), children.end(), [&](const std::unique_ptr<IRemoteObject>& lhs) {
		return lhs.get() == child.ptr();
	});
	if (itr != children.end()) {
		std::unique_ptr<IRemoteObject> childObject = std::move(*itr);
		children.erase(itr);
		return childObject;
	}
	return nullptr;
}

template<typename RuntimeType, typename DebugVisualType>
void IRemoteInstance<RuntimeType, DebugVisualType>::reparent(Reference<IRemoteObject> remoteObject) {
	parent = remoteObject;
}

template<typename RuntimeType, typename DebugVisualType>
void IRemoteInstance<RuntimeType, DebugVisualType>::add_child(std::unique_ptr<IRemoteObject> child) {
	if (!child) {
		return;
	}
	child->reparent(this);
	children.emplace_back(std::move(child));
}

template<typename RuntimeType, typename DebugVisualType>
void IRemoteInstance<RuntimeType, DebugVisualType>::on_spawn() {
	for (auto& child : children) {
		if (child) {
			child->on_spawn();
		}
	}
}

template<typename RuntimeType, typename DebugVisualType>
void IRemoteInstance<RuntimeType, DebugVisualType>::on_destroy() {
	for (auto& child : children) {
		if (child) {
			child->on_destroy();
		}
	}
}

}; // szg

#endif // DEBUG_FEATURES_ENABLE
