/*
 * AudioEngine.cpp
 *
 *  Created on: 24 jul 2026
 *      Author: DjSteker
 */

#include "AudioEngine.hpp"
#include "WavDecoder.hpp"
#include "Mp3Decoder.hpp"
#include "OggDecoder.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

AudioEngine::AudioEngine() {
	handle_ = nullptr;
	playing_ = false;
	paused_ = false;
	stop_requested_ = false;
	sample_rate_ = 44100;
	channels_ = 2;
	volume_ = 1.0f;
	streaming_mode_ = false;
	progress_callback_ = nullptr;
	progress_user_data_ = nullptr;
	data_size_ = 0;
	bytes_read_ = 0;
	on_finished_callback = []() {
	};
}

AudioEngine::~AudioEngine() {
	stop();
	close();
}

bool AudioEngine::init(const std::string &device) {
	std::lock_guard<std::mutex> lock(audio_mutex_);
	closeLocked();

	int err = snd_pcm_open(&handle_, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		std::cerr << "Error ALSA: " << snd_strerror(err) << std::endl;
		handle_ = nullptr;
		return false;
	}

	snd_pcm_hw_params_t *params;
	snd_pcm_hw_params_alloca(&params);
	snd_pcm_hw_params_any(handle_, params);
	snd_pcm_hw_params_set_access(handle_, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(handle_, params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_rate_near(handle_, params, (unsigned int*) &sample_rate_, 0);
	snd_pcm_hw_params_set_channels(handle_, params, channels_);

	unsigned int buffer_time = 500000;
	snd_pcm_hw_params_set_buffer_time_near(handle_, params, &buffer_time, 0);

	err = snd_pcm_hw_params(handle_, params);
	if (err < 0) {
		std::cerr << "Error config ALSA: " << snd_strerror(err) << std::endl;
		snd_pcm_close(handle_);
		handle_ = nullptr;
		return false;
	}
	return true;
}

void AudioEngine::close() {
	std::lock_guard<std::mutex> lock(audio_mutex_);
	closeLocked();
}

// Versión interna: asume que audio_mutex_ ya está bloqueado por el llamante.
// Nunca debe bloquear el mutex ella misma ni ser llamada sin tenerlo cogido.
void AudioEngine::closeLocked() {
	if (handle_) {
		snd_pcm_close(handle_);
		handle_ = nullptr;
	}
}

bool AudioEngine::loadFile(const std::string &filepath) {
	stop();
	std::lock_guard<std::mutex> lock(audio_mutex_);

	streaming_mode_ = false;

	std::string ext = fs::path(filepath).extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	std::unique_ptr<IAudioDecoder> new_decoder;
	if (ext == ".wav") {
		new_decoder = std::make_unique<WavDecoder>();
	} else if (ext == ".mp3") {
		new_decoder = std::make_unique<Mp3Decoder>();
	} else if (ext == ".ogg") {
		new_decoder = std::make_unique<OggDecoder>();
	} else {
		std::cerr << "Formato de audio no soportado: " << ext << std::endl;
		return false;
	}

	if (!new_decoder->open(filepath)) {
		std::cerr << "Error al abrir archivo de audio: " << filepath << std::endl;
		return false;
	}

	decoder_ = std::move(new_decoder);
	channels_ = decoder_->channels();
	sample_rate_ = decoder_->sampleRate();
	bytes_read_ = 0;

	// El tamaño del archivo en disco es una aproximación razonable para el
	// progreso en formatos comprimidos (no es el tamaño exacto de PCM
	// decodificado, pero basta para una barra de progreso orientativa).
	std::error_code ec;
	data_size_ = fs::file_size(filepath, ec);
	if (ec) {
		data_size_ = 0;
	}

	if (handle_) {
		snd_pcm_hw_params_t *params;
		snd_pcm_hw_params_alloca(&params);
		snd_pcm_hw_params_current(handle_, params);
		snd_pcm_hw_params_set_rate_near(handle_, params, (unsigned int*) &sample_rate_, 0);
		snd_pcm_hw_params_set_channels(handle_, params, channels_);
		snd_pcm_hw_params(handle_, params);
		snd_pcm_prepare(handle_);
	}

	streaming_mode_ = true;
	return true;
}

bool AudioEngine::fillBuffer(char *buffer, size_t buffer_size) {
	if (!streaming_mode_ || !decoder_) {
		return false;
	}

	size_t read = decoder_->readPcm(buffer, buffer_size);
	if (read == 0) {
		return false;
	}

	bytes_read_ += read;

	if (volume_ < 0.99f || volume_ > 1.01f) {
		int16_t *samples = reinterpret_cast<int16_t*>(buffer);
		size_t num_samples = read / sizeof(int16_t);
		for (size_t i = 0; i < num_samples; ++i) {
			samples[i] = static_cast<int16_t>(samples[i] * volume_);
		}
	}

	if (progress_callback_ && data_size_ > 0) {
		double progress = static_cast<double>(bytes_read_) / static_cast<double>(data_size_);
		progress_callback_(std::min(progress, 1.0), progress_user_data_);
	}

	// El buffer se pudo rellenar parcialmente (read < buffer_size); ALSA
	// solo recibe lo realmente decodificado, vía frames en playbackLoop().
	if (read < buffer_size) {
		std::memset(buffer + read, 0, buffer_size - read);
	}

	return true;
}

void AudioEngine::play() {
	if (!handle_) {
		return;
	}
	if (!streaming_mode_) {
		return;
	}

	playing_ = true;
	paused_ = false;
	stop_requested_ = false;

	if (playback_thread_.joinable()) {
		playback_thread_.join();
	}
	playback_thread_ = std::thread(&AudioEngine::playbackLoop, this);
}

void AudioEngine::pause() {
	if (playing_) {
		paused_ = !paused_;
	}
}

void AudioEngine::stop() {
	stop_requested_ = true;
	playing_ = false;
	if (playback_thread_.joinable()) {
		playback_thread_.join();
	}
	std::lock_guard<std::mutex> lock(audio_mutex_);
	if (handle_) {
		snd_pcm_drop(handle_);
	}
}

bool AudioEngine::isPlaying() const {
	return playing_ && !paused_;
}

bool AudioEngine::isPaused() const {
	return playing_ && paused_;
}

void AudioEngine::setVolume(float volume) {
	volume_ = std::clamp(volume, 0.0f, 1.0f);
}

void AudioEngine::setProgressCallback(ProgressCallback callback, void *user_data) {
	progress_callback_ = callback;
	progress_user_data_ = user_data;
}

void AudioEngine::playbackLoop() {
	char pcm_buffer[8192];
	size_t bytes_per_frame = channels_ * 2;

	while (!stop_requested_) {
		if (paused_) {
			usleep(100000);
			continue;
		}

		if (!fillBuffer(pcm_buffer, sizeof(pcm_buffer))) {
			break;
		}

		snd_pcm_sframes_t frames = snd_pcm_writei(handle_, pcm_buffer, sizeof(pcm_buffer) / bytes_per_frame);
		if (frames < 0) {
			frames = snd_pcm_recover(handle_, frames, 0);
			if (frames < 0) {
				std::cerr << "Error crítico ALSA" << std::endl;
				break;
			}
		}
	}

	playing_ = false;
	if (!stop_requested_) {
		if (on_finished_callback) {
			on_finished_callback();
		}
	}
}
