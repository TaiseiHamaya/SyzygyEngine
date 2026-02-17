#include "SkeletonAssetBuilder.h"

#include "./SkeletonAsset.h"
#include "./SkeletonLibrary.h"
#include "Engine/Application/Logger.h"

#include <Library/Math/Quaternion.h>
#include <Library/Utility/Tools/SmartPointer.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace szg;

// 前方宣言
static Affine ToAffine(const aiMatrix4x4& aiMatrix);
static bool CollectBoneNode(
	std::unordered_map<std::string, u32>& nodeElementMap,
	const aiNode* node,
	const std::unordered_map<std::string, aiBone*>& boneElementsName
);
static std::optional<u32> CreateSkeleton(
	Skeleton& skeleton,
	const std::optional<u32>& parentIndex,
	const aiNode* node,
	std::unordered_map<std::string, u32>& nodeElementMap
);

// ---------- ここから本編 ----------

SkeletonAssetBuilder::SkeletonAssetBuilder(const std::filesystem::path& filePath_) {
	filePath = IAssetBuilder::ResolveFilePath(filePath_);
}

bool SkeletonAssetBuilder::run() {
	Skeleton skeleton;
	std::unordered_map<std::string, VertexBuffer<VertexInfluenceData>> influenceBuffers;

	Assimp::Importer importer;
	// 読み込み
	const aiScene* scene = importer.ReadFile(filePath.string().c_str(),
		aiProcess_FlipUVs |
		aiProcess_FlipWindingOrder |
		aiProcess_LimitBoneWeights
	);
	// ロード失敗時
	if (importer.GetException() || !scene) {
		szgError("Failed to load skeleton file. File-\'{}\' Message-\'{}\'", filePath.string(), importer.GetErrorString());
		return false;
	}
	// スケルトン存在しない
	if (!scene->mRootNode || !scene->mNumMeshes) {
		szgError("Can't find skeletons. File-\'{}\'", filePath.string());
		return false;
	}

	// 使用するボーンの取得
	std::unordered_map<std::string, aiBone*> boneElementsName;
	for (u32 meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* pAiMesh = scene->mMeshes[meshIndex];
		if (!pAiMesh->HasBones()) {
			continue;
		}
		for (u32 boneIndex = 0; boneIndex < pAiMesh->mNumBones; ++boneIndex) {
			aiBone* bone = pAiMesh->mBones[boneIndex];
			boneElementsName.try_emplace(bone->mName.C_Str(), bone);
		}
	}

	// 使用するNode一覧を取得
	std::unordered_map<std::string, u32> nodeElementMap;
	CollectBoneNode(nodeElementMap, scene->mRootNode, boneElementsName);

	// Node一覧から階層構造を取得
	skeleton.joints.reserve(nodeElementMap.size());
	CreateSkeleton(skeleton, std::nullopt, scene->mRootNode, nodeElementMap);

	// SkeletonにInverseBindPoseMatrixを書き込み
	for (Joint& joint : skeleton.joints) {
		if (!boneElementsName.contains(joint.name)) {
			continue;
		}
		aiBone* bone = boneElementsName.at(joint.name);
		joint.inverseBindPoseAffine = ToAffine(bone->mOffsetMatrix);
	}

	// Meshで使用するNodeとJointのIndexを関連付け
	for (u32 meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		const aiMesh* mesh = scene->mMeshes[meshIndex];
		std::vector<u32>& useJointIndexes = skeleton.useJointIndexesByMeshName[mesh->mName.C_Str()];
		for (u32 boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			// Bone取得
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			useJointIndexes.emplace_back(nodeElementMap.at(boneName));
		}
	}

	// Weightの書き込み
	for (u32 meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		const aiMesh* mesh = scene->mMeshes[meshIndex];
		// Weight用Bufferの生成
		std::vector<VertexInfluenceData> influenceData;
		std::vector<u32> influenceIndexes; // ↑の次のIndexを参照
		influenceData.resize(mesh->mNumVertices);
		influenceIndexes.resize(mesh->mNumVertices, 0);
		for (u32 boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			// Bone取得
			aiBone* bone = mesh->mBones[boneIndex];
			// 書き込み用データ作成
			for (u32 i = 0; i < bone->mNumWeights; ++i) {
				u32 vertexIndex = bone->mWeights[i].mVertexId; // 頂点Index
				u32& influenceIndex = influenceIndexes[vertexIndex]; // 書き込み先インデックス
				VertexInfluenceData& influence = influenceData[vertexIndex]; // 書き込み先データ

				if (influenceIndex < VertexInfluenceData::NumMaxInfluence) {
					influence.weights[influenceIndex] = bone->mWeights[i].mWeight;
					influence.jointIndex[influenceIndex] = boneIndex;
					++influenceIndex;
				}
			}
		}
		// GPUに転送
		VertexBuffer<VertexInfluenceData>& influenceBuffer = influenceBuffers[mesh->mName.C_Str()];
		influenceBuffer.write(std::move(influenceData));
	}

	skeletonData = eps::CreateShared<SkeletonAsset>(skeleton, influenceBuffers);

	return true;
}

void SkeletonAssetBuilder::postprocess() {
	// Do nothing
}

void SkeletonAssetBuilder::transfer() {
	SkeletonLibrary::Transfer(filePath.filename().string(), skeletonData);
}

// ---------- ここまで本編 ----------

static Affine ToAffine(const aiMatrix4x4& aiMatrix) {
	aiVector3D scale;
	aiQuaternion rotate;
	aiVector3D translate;
	aiMatrix.Decompose(scale, rotate, translate);

	return Affine::FromSRT(
		{ scale.x,scale.y,scale.z },
		Quaternion{ rotate.x, -rotate.y, -rotate.z, rotate.w },
		{ -translate.x,translate.y,translate.z }
	);
}

// 行きがけDFSによるNodeの全探索
static std::optional<u32> CreateSkeleton(
	Skeleton& skeleton, // 生成中のSkeleton
	const std::optional<u32>& parentIndex, // 親Index
	const aiNode* node, // 今のNode
	std::unordered_map<std::string, u32>& nodeElementMap // ボーン一覧
) {
	std::optional<u32> thisIndex;
	std::string name = node->mName.C_Str();
	auto& joints = skeleton.joints;
	Joint* joint = nullptr;
	if (nodeElementMap.contains(name)) {
		// Jointの追加
		joint = &joints.emplace_back(
			name,
			Affine{},
			parentIndex,
			std::vector<u32>{}
		);

		thisIndex = static_cast<u32>(joints.size()) - 1;

		nodeElementMap.at(name) = thisIndex.value();
	}

	for (u32 nextNodeIndex = 0; nextNodeIndex < node->mNumChildren; ++nextNodeIndex) {
		// 再帰的実行
		std::optional<u32> result = CreateSkeleton(skeleton, thisIndex, node->mChildren[nextNodeIndex], nodeElementMap);
		// 要素が有効な場合のみ追加
		if (result && joint) {
			joint->children.emplace_back(result.value());
		}
	}
	return thisIndex;
}

static bool CollectBoneNode(
	std::unordered_map<std::string, u32>& nodeElementMap,
	const aiNode* node,
	const std::unordered_map<std::string, aiBone*>& boneElementsName) {
	bool result = false;
	std::string name = node->mName.C_Str();
	if (boneElementsName.contains(name)) {
		result = true;
	}
	for (u32 nextNodeIndex = 0; nextNodeIndex < node->mNumChildren; ++nextNodeIndex) {
		// 再帰的実行
		result = CollectBoneNode(nodeElementMap, node->mChildren[nextNodeIndex], boneElementsName) || result;
	}

	if (result) {
		nodeElementMap.emplace(name, 0);
	}

	return result;
}
