#ifdef DEBUG_FEATURES_ENABLE

#include "EditorAssetField.h"

#include "Engine/Debug/Editor/Command/EditorCommandRegisterAsset.h"
#include "Engine/Debug/Editor/Command/EditorCommandScope.h"
#include "Engine/Debug/Editor/Command/EditorValueChangeCommandHandler.h"
#include "Engine/Debug/Editor/Core/EditorAssetContentsCollector.h"
#include "Engine/Debug/Editor/Core/EditorSceneAssetCollection.h"

szg::EditorAssetField::EditorAssetField(const std::string& label_, AssetType type, std::string init) :
	label(label_),
	value(init),
	assetType(type) {
}

std::bitset<2> szg::EditorAssetField::show_gui(std::function<void(void)> preprocess, std::function<void(void)> postporcess) {
	auto result = EditorAssetContentsCollector::ComboGUI(value, assetType, label);

	if (result.has_value()) {
		// Assetの切り替えがあった場合、古いAssetの登録を解除して新しいAssetを登録する

		// Assetのフルパスを取得
		auto path = EditorAssetContentsCollector::GetAssetPath(assetType, value);

		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeBegin>());

		if (preprocess) {
			preprocess();
		}

		// 古いAssetの登録を解除
		if (path) {
			EditorCommandInvoker::Execute(std::make_unique<EditorCommandUnregisterAsset>(assetType, path.value()));
		}
		// 新しいAssetの登録
		EditorCommandInvoker::Execute(std::make_unique<EditorCommandRegisterAsset>(assetType, result.value().path));
		// 値の変更
		EditorValueChangeCommandHandler::GenCommandInstant<std::string>(value, result.value().fileName);

		if (postporcess) {
			postporcess();
		}

		EditorCommandInvoker::Execute(std::make_unique<EditorCommandScopeEnd>());
		return 0b10;
	}
	else {
		return 0b00;
	}
}

void szg::EditorAssetField::set_weak(const std::string& value_) {
	value = value_;
}

szg::EditorAssetField::operator const std::string& () const noexcept {
	return value;
}

void szg::EditorAssetField::on_activated() {
	auto path = EditorAssetContentsCollector::GetAssetPath(assetType, value);
	if (path) {
		EditorSceneAssetCollection::RegisterAsset(assetType, path.value());
	}
}

void szg::EditorAssetField::on_deactivated() {
	auto path = EditorAssetContentsCollector::GetAssetPath(assetType, value);
	if (path) {
		EditorSceneAssetCollection::UnregisterAsset(assetType, path.value());
	}
}

#endif // DEBUG_FEATURES_ENABLE
