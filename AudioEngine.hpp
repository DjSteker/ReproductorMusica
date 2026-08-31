/*
 * AudioEngine.hpp
 *
 *  Created on: 24 jul 2026
 *      Author: DjSteker
 */

/**
 * @file AudioEngine.hpp
 * @brief Motor de audio para reproducción de WAV/MP3/OGG usando ALSA
 * @author DjSteker
 * @date 31 de agosto de 2026
 * @version 2.0
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
#include <memory>
#include "IAudioDecoder.hpp"

/**
 * @typedef ProgressCallback
 * @brief Callback de tipo C para notificar el progreso de reproducción
 * @param progress Valor de progreso entre 0.0 y 1.0
 * @param user_data Puntero a datos del usuario
 */
typedef void (*ProgressCallback)(double progress, void *user_data);

/**
 * @class AudioEngine
 * @brief Gestor de reproducción de audio usando ALSA (Advanced Linux Sound Architecture)
 *
 * Proporciona funcionalidad para cargar, reproducir, pausar y controlar
 * archivos de audio. Es agnóstico respecto al formato del archivo: delega
 * la decodificación en una implementación de IAudioDecoder elegida según
 * la extensión del archivo (WavDecoder, Mp3Decoder, OggDecoder), y ella
 * misma solo se encarga de leer bloques de PCM del decoder activo y
 * enviarlos a ALSA desde un hilo de reproducción dedicado.
 */
class AudioEngine {
public:
	/**
	 * @brief Constructor del motor de audio
	 */
	AudioEngine();

	/**
	 * @brief Destructor del motor de audio
	 * Libera recursos y detiene la reproducción
	 */
	~AudioEngine();

	/**
	 * @brief Inicializa el motor de audio con un dispositivo específico
	 * @param device Nombre del dispositivo ALSA (por defecto "default")
	 * @return true si la inicialización fue exitosa, false en caso contrario
	 */
	bool init(const std::string &device = "default");

	/**
	 * @brief Cierra y libera los recursos del motor de audio
	 */
	void close();

	/**
	 * @brief Carga un archivo de audio para reproducción
	 *
	 * El formato se determina por la extensión del archivo (.wav, .mp3,
	 * .ogg) y se instancia el IAudioDecoder correspondiente.
	 *
	 * @param filepath Ruta del archivo de audio a cargar
	 * @return true si la carga fue exitosa, false si el formato no está
	 * soportado o el archivo no pudo abrirse/decodificarse
	 */
	bool loadFile(const std::string &filepath);

	/**
	 * @brief Inicia la reproducción del archivo cargado
	 */
	void play();

	/**
	 * @brief Pausa la reproducción actual
	 */
	void pause();

	/**
	 * @brief Detiene la reproducción y reinicia la posición
	 */
	void stop();

	/**
	 * @brief Verifica si el audio se está reproduciendo
	 * @return true si está reproduciendo, false en caso contrario
	 */
	bool isPlaying() const;

	/**
	 * @brief Verifica si la reproducción está pausada
	 * @return true si está pausada, false en caso contrario
	 */
	bool isPaused() const;

	/**
	 * @brief Establece el volumen de reproducción
	 * @param volume Volumen (rango típico 0.0 a 1.0)
	 */
	void setVolume(float volume);

	/**
	 * @brief Callback cuando la reproducción finaliza
	 */
	std::function<void()> on_finished_callback;

	/**
	 * @brief Establece un callback de progreso en estilo C
	 * @param callback Función callback para el progreso
	 * @param user_data Puntero a datos del usuario
	 */
	void setProgressCallback(ProgressCallback callback, void *user_data);

private:
	/**
	 * @brief Manejador del dispositivo PCM de ALSA
	 */
	snd_pcm_t *handle_;

	/**
	 * @brief Bandera atómica indicando reproducción activa
	 */
	std::atomic<bool> playing_;

	/**
	 * @brief Bandera atómica indicando pausa
	 */
	std::atomic<bool> paused_;

	/**
	 * @brief Bandera para solicitar detención del playback
	 */
	std::atomic<bool> stop_requested_;

	/**
	 * @brief Thread que ejecuta el loop de reproducción
	 */
	std::thread playback_thread_;

	/**
	 * @brief Mutex para sincronización de audio
	 */
	std::mutex audio_mutex_;

	/**
	 * @brief Decodificador activo para el archivo actualmente cargado
	 *
	 * Se elige e instancia en loadFile() según la extensión del archivo.
	 * AudioEngine no conoce el formato concreto, solo pide bloques de
	 * PCM a través de la interfaz IAudioDecoder.
	 */
	std::unique_ptr<IAudioDecoder> decoder_;

	/**
	 * @brief Frecuencia de muestreo en Hz del archivo cargado
	 */
	int sample_rate_;

	/**
	 * @brief Número de canales (1=mono, 2=estéreo) del archivo cargado
	 */
	int channels_;

	/**
	 * @brief Nivel de volumen actual
	 */
	float volume_;

	/**
	 * @brief Modo de streaming (true cuando hay un decoder cargado y listo)
	 */
	bool streaming_mode_;

	/**
	 * @brief Callback de progreso en estilo C
	 */
	ProgressCallback progress_callback_;

	/**
	 * @brief Datos de usuario para el callback
	 */
	void *progress_user_data_;

	/**
	 * @brief Tamaño total en bytes de los datos de audio del archivo cargado
	 *
	 * Usado únicamente para calcular el progreso (progress_callback_).
	 * Lo reporta el decoder indirectamente: se recalcula por tamaño de
	 * archivo cuando el decoder no lo expone directamente.
	 */
	size_t data_size_;

	/**
	 * @brief Bytes de PCM decodificados hasta el momento
	 */
	size_t bytes_read_;

	/**
	 * @brief Loop principal de reproducción de audio
	 */
	void playbackLoop();

	/**
	 * @brief Rellena un buffer con datos de audio pedidos al decoder activo
	 * @param buffer Puntero al buffer a rellenar
	 * @param buffer_size Tamaño del buffer
	 * @return true si se leyeron datos correctamente
	 */
	bool fillBuffer(char *buffer, size_t buffer_size);

	/**
	 * @brief Versión interna de close() sin bloqueo de mutex
	 * Asume que el llamante ya ha bloqueado audio_mutex_.
	 * Evita el deadlock al bloquear un mutex no-reentrante dos veces.
	 */
	void closeLocked();
};

#endif /* AUDIOENGINE_HPP_ */
