#include "AudioAsset.h"

using namespace szg;

AudioAsset::AudioAsset(WAVEFORMATEXTENSIBLE format_, std::vector<u8> buffer_, u32 bufferSize_) :
	waveFormat(format_),
	buffer(buffer_),
	bufferSize(bufferSize_) {
}

u32 AudioAsset::buffer_size() const noexcept {
	return bufferSize;
}

const std::vector<u8>& AudioAsset::buffer_data() const noexcept {
	return buffer;
}

const WAVEFORMATEX& AudioAsset::format() const noexcept {
	return waveFormat.Format;
}
