#include "AudioPlayer.h"

#include "Engine/Application/Logger.h"
#include "Engine/Assets/Audio/AudioAsset.h"
#include "Engine/Assets/Audio/AudioLibrary.h"
#include "Engine/Assets/Audio/AudioManager.h"

using namespace szg;

AudioPlayer::~AudioPlayer() {
	destroy();
}

void AudioPlayer::initialize(const std::string& name, r32 volume, bool isLoop) {
	auto&& audio = AudioLibrary::GetAudio(name);
	if (!audio) {
		return;
	}
	HRESULT result;
	result = AudioManager::GetXAudio2()->CreateSourceVoice(&sourceVoice, &audio->format());
	szgErrorIf(FAILED(result), "Failed crating source voice. File-\'{}\'", name);

	buffer.pAudioData = audio->buffer_data().data();
	buffer.AudioBytes = audio->buffer_size();
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = isLoop ? XAUDIO2_LOOP_INFINITE : 0;
	sourceVoice->SetVolume(volume);
	result = sourceVoice->SubmitSourceBuffer(&buffer);
	szgErrorIf(FAILED(result), "Failed submit source buffer. File-\'{}\'", name);
}

void AudioPlayer::destroy() {
	if (sourceVoice) {
		pause();
		sourceVoice->DestroyVoice();
	}
	sourceVoice = nullptr;
}

void AudioPlayer::play() {
	if (!sourceVoice) {
		szgWarning("Called \'play\' function on not initialized audio player. Address-\'{}\'", (void*)this);
		return;
	}
	HRESULT result;
	result = sourceVoice->Start();
	szgErrorIf(FAILED(result), "Start function was called but failed for unknown reason. Address-\'{}\'", (void*)this);
}

void AudioPlayer::stop() {
	if (!sourceVoice) {
		szgWarning("Called \'stop\' function on not initialized audio player. Address-\'{}\'", (void*)this);
		return;
	}
	HRESULT result;
	result = sourceVoice->Stop();
	szgErrorIf(FAILED(result), "Stop function was called but failed for unknown reason. Address-\'{}\'", (void*)this);
	sourceVoice->FlushSourceBuffers();
	sourceVoice->SubmitSourceBuffer(&buffer);
}

void AudioPlayer::pause() {
	if (!sourceVoice) {
		szgWarning("Called \'pause\' function on not initialized audio player. Address-\'{}\'", (void*)this);
		return;
	}
	HRESULT result;
	result = sourceVoice->Stop();
	szgErrorIf(FAILED(result), "Pause function was called but failed for unknown reason. Address-\'{}\'", (void*)this);
}

void AudioPlayer::restart() {
	stop();
	play();
}

void AudioPlayer::set_volume(r32 volume) {
	if (!sourceVoice) {
		szgWarning("Called \'set_volume\' function on not initialized audio player. Address-\'{}\'", (void*)this);
		return;
	}
	sourceVoice->SetVolume(volume);
}

void AudioPlayer::set_loop(bool isLoop) {
	if (!sourceVoice) {
		szgWarning("Called \'set_loop\' function on not initialized audio player. Address-\'{}\'", (void*)this);
		return;
	}
	buffer.LoopCount = isLoop ? XAUDIO2_LOOP_INFINITE : 0;
	HRESULT result;
	result = sourceVoice->SubmitSourceBuffer(&buffer);
}

#ifdef DEBUG_FEATURES_ENABLE
#include <imgui.h>
void AudioPlayer::debug_gui() {
	if (!sourceVoice) {
		return;
	}
	bool isLoop = buffer.LoopCount == XAUDIO2_LOOP_INFINITE;
	if (ImGui::Checkbox("Loop", &isLoop)) {
		set_loop(isLoop);
	}
	r32 volume;
	sourceVoice->GetVolume(&volume);
	if (ImGui::DragFloat("Volume", &volume, 0.01f, 0.0f, 100.0f)) {
		set_volume(volume);
	}
}
#endif // _DEBUG
