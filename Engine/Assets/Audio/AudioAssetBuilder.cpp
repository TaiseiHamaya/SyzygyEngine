#include "AudioAssetBuilder.h"

#include <array>
#include <cstring>
#include <fstream>

#include "./AudioAsset.h"
#include "./AudioLibrary.h"
#include "Engine/Application/Logger.h"

#include <Library/Utility/Tools/SmartPointer.h>

using namespace szg;

bool IsEqualArrayChunkId(const std::array<i8, 4>& read, std::string&& id) {
	return std::strncmp(read.data(), id.c_str(), 4) == 0;
}

AudioAssetBuilder::AudioAssetBuilder(const std::filesystem::path& filePath_) {
	filePath = IAssetBuilder::ResolveFilePath(filePath_, "Audio");
}

bool AudioAssetBuilder::run() {
	// ローカル変数定義
	struct ChunkHeader {
		std::array<i8, 4> id;
		i32 size;
	};
	struct RiffHeader {
		ChunkHeader header;
		std::array<i8, 4> type;
	};
	struct FormatChunk {
		ChunkHeader header;
		WAVEFORMATEXTENSIBLE format;
	};

	szgInformation("Start load .wave file. file-\'{}\'", filePath.string());

	// ファイル読み込み
	std::ifstream file;
	file.open(filePath, std::ios_base::binary);

	// 失敗したら何もしない
	if (!file.is_open()) {
		szgError("Failed open file. \'{}\'", filePath.filename().string());
		return false;
	}

	// riff読み込み
	RiffHeader riff;
	file.read((i8*)&riff, sizeof(riff));

	// RIFFじゃなかったらエラー処理
	if (!IsEqualArrayChunkId(riff.header.id, "RIFF")) {
		szgError("Failed loading. File \'{}\' don't have RIFF chunk.", filePath.filename().string());
		return false;
	}

	// WAVEフォーマットじゃなかったらエラー処理
	if (!IsEqualArrayChunkId(riff.type, "WAVE")) {
		szgError("Failed loading. File \'{}\' is not WAVE format.", filePath.filename().string());
		return false;
	}

	// chunk読み込み
	FormatChunk formatChunk{};
	while (file.read((i8*)&formatChunk.header, sizeof(ChunkHeader))) {
		if (IsEqualArrayChunkId(formatChunk.header.id, "fmt ")) {
			break;
		}
		else if (IsEqualArrayChunkId(formatChunk.header.id, "JUNK")) {
			file.seekg(formatChunk.header.size, std::ios_base::cur);
		}
		else if (IsEqualArrayChunkId(formatChunk.header.id, "LIST")) {
			file.seekg(formatChunk.header.size, std::ios_base::cur);
		}
		else {
			file.seekg(formatChunk.header.size, std::ios_base::cur);
		}
	}
	// chunk.sizeよりformatが小さくないとread時にエラーになる
	if (formatChunk.header.size > sizeof(formatChunk.format)) {
		szgError("This fmt chunk format is not support. Size-\'{}\'", formatChunk.header.size);
		return false;
	}

	// 読み込み
	file.read((i8*)&formatChunk.format, formatChunk.header.size);
	// データ読み込み
	ChunkHeader data;
	while (file.read((i8*)&data, sizeof(data))) {
		if (IsEqualArrayChunkId(data.id, "data")) {
			break;
		}
		else if (IsEqualArrayChunkId(data.id, "JUNK")) {
			file.seekg(data.size, std::ios_base::cur);
		}
		else if (IsEqualArrayChunkId(data.id, "LIST")) {
			file.seekg(data.size, std::ios_base::cur);
		}
		else {
			file.seekg(data.size, std::ios_base::cur);
		}
	}

	// dataチャンクがなかったらエラー
	if (!IsEqualArrayChunkId(data.id, "data")) {
		szgError("Failed loading. \'data\' chunk is not found. File-\'{}\'", filePath.string());
		return false;
	}

	// 読み込んだデータをコピーして保持
	std::vector<u8> buffer_(data.size);
	file.read((i8*)buffer_.data(), data.size);

	file.close();

	audioData = eps::CreateUnique<AudioAsset>(
		formatChunk.format,
		std::move(buffer_),
		data.size
	);

	szgInformation("Succeeded.");
	return true;
}

void AudioAssetBuilder::postprocess() {
	// Do nothing
}

void AudioAssetBuilder::transfer() {
	AudioLibrary::Transfer(filePath.filename().string(), std::move(audioData));
}
