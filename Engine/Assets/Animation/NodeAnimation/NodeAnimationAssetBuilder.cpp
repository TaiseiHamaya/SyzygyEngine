#include "NodeAnimationAssetBuilder.h"

#include <Library/Utility/Tools/SmartPointer.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "./NodeAnimationAsset.h"
#include "./NodeAnimationLibrary.h"
#include "Engine/Application/Logger.h"

using namespace szg;

NodeAnimationAssetBuilder::NodeAnimationAssetBuilder(const std::filesystem::path& filePath_) {
	filePath = IAssetBuilder::ResolveFilePath(filePath_);
}

bool NodeAnimationAssetBuilder::run() {
	Assimp::Importer importer;
	// 読み込み
	const aiScene* scene = importer.ReadFile(filePath.string().c_str(),
		aiProcess_FlipUVs |
		aiProcess_FlipWindingOrder |
		aiProcess_LimitBoneWeights
	);
	// ロード失敗時
	if (importer.GetException() || !scene) {
		szgError("Failed to load animation file. File-\'{}\' Message-\'{}\'", filePath.string(), importer.GetErrorString());
		return false;
	}
	// アニメーションが存在しない
	if (!scene->HasAnimations()) {
		szgError("Can't find animation. File-\'{}\'", filePath.string());
		return false;
	}

	nodeAnimationData.resize(scene->mNumAnimations);

	std::string fileName = filePath.filename().string();

	for (u32 i = 0; i < scene->mNumAnimations; ++i) {
		BuildData& write = nodeAnimationData[i];
		aiAnimation* aiPAnimation = scene->mAnimations[i];
		write.name = std::format("{}-{}", fileName, aiPAnimation->mName.C_Str());

		r32 ticksPerSecond = static_cast<r32>(aiPAnimation->mTicksPerSecond);
		// duration算出
		r32 duration = r32(aiPAnimation->mDuration) / ticksPerSecond;
		std::unordered_map<std::string, NodeAnimationAsset::NodeAnimation> nodeAnimations;
		for (u32 channelIndex = 0; channelIndex < aiPAnimation->mNumChannels; ++channelIndex) {
			aiNodeAnim* aiNodeAnimation = aiPAnimation->mChannels[channelIndex];
			// Node追加
			NodeAnimationAsset::NodeAnimation& nodeAnimation = nodeAnimations[aiNodeAnimation->mNodeName.C_Str()];
			// Scale
			for (u32 keyIndex = 0; keyIndex < aiNodeAnimation->mNumScalingKeys; ++keyIndex) {
				aiVectorKey& aiKey = aiNodeAnimation->mScalingKeys[keyIndex];
				r32 time = r32(aiKey.mTime) / ticksPerSecond;
				Vector3 value = { aiKey.mValue.x,aiKey.mValue.y,aiKey.mValue.z };
				nodeAnimation.scale.keyframes.emplace(time, value);
			}
			// Rotation
			for (u32 keyIndex = 0; keyIndex < aiNodeAnimation->mNumRotationKeys; ++keyIndex) {
				aiQuatKey& aiKey = aiNodeAnimation->mRotationKeys[keyIndex];
				r32 time = r32(aiKey.mTime) / ticksPerSecond;
				Quaternion value = { aiKey.mValue.x, -aiKey.mValue.y, -aiKey.mValue.z, aiKey.mValue.w };
				nodeAnimation.rotate.keyframes.emplace(time, value);
			}
			// Translate
			for (u32 keyIndex = 0; keyIndex < aiNodeAnimation->mNumPositionKeys; ++keyIndex) {
				aiVectorKey& aiKey = aiNodeAnimation->mPositionKeys[keyIndex];
				r32 time = r32(aiKey.mTime) / ticksPerSecond;
				Vector3 value = { -aiKey.mValue.x,aiKey.mValue.y,aiKey.mValue.z };
				nodeAnimation.translate.keyframes.emplace(time, value);
			}
		}
		write.asset = eps::CreateShared<NodeAnimationAsset>(duration, nodeAnimations);
	}

	return true;
}

void NodeAnimationAssetBuilder::postprocess() {
	// Do nothing
}

void NodeAnimationAssetBuilder::transfer() {
	for (const BuildData& data : nodeAnimationData) {
		NodeAnimationLibrary::Transfer(data.name, data.asset);
	}
}
