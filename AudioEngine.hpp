/*
 * AudioEngine.hpp
 *
 *  Created on: 24 jul 2026
 *      Author: DjSteker
 */

#ifndef AUDIOENGINE_HPP_
#define AUDIOENGINE_HPP_

#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <fstream>

// Definir el tipo de callback C-style
typedef void (*ProgressCallback)(double progress, void *user_data);

class AudioEngine {
public:
	AudioEngine();
	~AudioEngine();

	bool init(const std::string &device = "default");
	void close();
	bool loadWav(const std::string &filepath);
	void play();
	void pause();
	void stop();
	bool isPlaying() const;
	bool isPaused() const;
	void setVolume(float volume);
	std::function<void()> on_finished_callback;

	// Nuevo: Callback C-style para progreso
	void setProgressCallback(ProgressCallback callback, void *user_data);

private:
	snd_pcm_t *handle_;
	std::atomic<bool> playing_;
	std::atomic<bool> paused_;
	std::atomic<bool> stop_requested_;
	std::thread playback_thread_;
	std::mutex audio_mutex_;
	std::ifstream wav_file_;
	size_t data_offset_;
	size_t data_size_;
	size_t bytes_read_;
	int sample_rate_;
	int channels_;
	snd_pcm_format_t format_;
	float volume_;
	bool streaming_mode_;

	// Callback y datos de usuario para progreso
	ProgressCallback progress_callback_;
	void *progress_user_data_;

	bool parseWavHeader(std::ifstream &file, size_t &data_offset, size_t &data_size);
	void playbackLoop();
	bool fillBuffer(char *buffer, size_t buffer_size);

	// FIX: versión interna de close() que NO bloquea audio_mutex_ -- asume que
	// el llamante ya lo tiene bloqueado. init() la usa en vez de llamar a
	// close() para evitar el autodeadlock de bloquear un std::mutex no
	// reentrante dos veces desde el mismo hilo.
	void closeLocked();
};

#endif /* AUDIOENGINE_HPP_ */
