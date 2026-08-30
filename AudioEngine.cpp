/*
 * AudioEngine.cpp
 *
 *  Created on: 24 jul 2026
 *      Author: DjSteker
 */

#include "AudioEngine.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <algorithm>

AudioEngine::AudioEngine() {
	handle_ = nullptr;
	playing_ = false;
	paused_ = false;
	stop_requested_ = false;
	data_offset_ = 0;
	data_size_ = 0;
	bytes_read_ = 0;
	sample_rate_ = 44100;
	channels_ = 2;
	format_ = SND_PCM_FORMAT_S16_LE;
	volume_ = 1.0f;
	streaming_mode_ = false;
	progress_callback_ = nullptr;
	progress_user_data_ = nullptr;
	on_finished_callback = []() {
	};
}

AudioEngine::~AudioEngine() {
	stop();
	close();
}

bool AudioEngine::init(const std::string &device) {
	std::lock_guard<std::mutex> lock(audio_mutex_);
	// FIX: antes se llamaba aquí a close(), que vuelve a bloquear audio_mutex_
	// (un std::mutex no reentrante) desde el mismo hilo que ya lo tiene
	// bloqueado dos líneas más arriba -> autodeadlock permanente. init() se
	// quedaba colgado para siempre sin error ni mensaje. closeLocked() hace lo
	// mismo pero asumiendo que el mutex ya está bloqueado por el llamante.
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
	snd_pcm_hw_params_set_format(handle_, params, format_);
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

bool AudioEngine::parseWavHeader(std::ifstream &file, size_t &data_offset, size_t &data_size) {
	char header[44];
	if (!file.read(header, 44)) {
		return false;
	}
	if (std::memcmp(header, "RIFF", 4) != 0 || std::memcmp(header + 8, "WAVE", 4) != 0) {
		return false;
	}

	// FIX: leer los campos con memcpy en vez de reinterpret_cast directo sobre el
	// buffer de char. reinterpret_cast a uint16_t*/uint32_t* sobre un char[] puede
	// violar la regla de aliasing estricto y, en plataformas que no toleran acceso
	// desalineado (o con ciertas optimizaciones del compilador), producir lecturas
	// incorrectas o un crash. memcpy es siempre válido y el compilador lo optimiza
	// igual de bien.
	uint16_t audio_format = 0;
	uint16_t channels = 0;
	uint32_t sample_rate = 0;
	uint32_t data_size32 = 0;

	std::memcpy(&audio_format, header + 20, sizeof(audio_format));
	std::memcpy(&channels, header + 22, sizeof(channels));
	std::memcpy(&sample_rate, header + 24, sizeof(sample_rate));
	std::memcpy(&data_size32, header + 40, sizeof(data_size32));

	channels_ = channels;
	sample_rate_ = static_cast<int>(sample_rate);

	if (audio_format != 1) {
		return false;
	}

	data_size = data_size32;
	data_offset = 44;
	return true;
}

bool AudioEngine::loadWav(const std::string &filepath) {
	stop();
	std::lock_guard<std::mutex> lock(audio_mutex_);

	wav_file_.close();
	wav_file_.clear();
	wav_file_.open(filepath, std::ios::binary);

	if (!wav_file_.is_open()) {
		return false;
	}

	if (!parseWavHeader(wav_file_, data_offset_, data_size_)) {
		wav_file_.close();
		return false;
	}

	wav_file_.seekg(data_offset_);
	bytes_read_ = 0;
	streaming_mode_ = true;

	if (handle_) {
		snd_pcm_hw_params_t *params;
		snd_pcm_hw_params_alloca(&params);
		snd_pcm_hw_params_current(handle_, params);
		snd_pcm_hw_params_set_rate_near(handle_, params, (unsigned int*) &sample_rate_, 0);
		snd_pcm_hw_params_set_channels(handle_, params, channels_);
		snd_pcm_hw_params(handle_, params);
		snd_pcm_prepare(handle_);
	}
	return true;
}

bool AudioEngine::fillBuffer(char *buffer, size_t buffer_size) {
	if (!streaming_mode_ || !wav_file_.is_open()) {
		return false;
	}

	size_t remaining = data_size_ - bytes_read_;
	size_t to_read = std::min(buffer_size, remaining);

	if (to_read == 0) {
		return false;
	}

	if (!wav_file_.read(buffer, to_read)) {
		return false;
	}

	bytes_read_ += to_read;

	if (volume_ < 0.99f || volume_ > 1.01f) {
		int16_t *samples = reinterpret_cast<int16_t*>(buffer);
		size_t num_samples = to_read / sizeof(int16_t);
		for (size_t i = 0; i < num_samples; ++i) {
			samples[i] = static_cast<int16_t>(samples[i] * volume_);
		}
	}

	if (progress_callback_ && data_size_ > 0) {
		double progress = static_cast<double>(bytes_read_) / data_size_;
		progress_callback_(progress, progress_user_data_);
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
