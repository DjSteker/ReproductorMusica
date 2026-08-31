/*
 * IAudioDecoder.hpp
 *
 *  Created on: 31 ago 2026
 *      Author: DjSteker
 */

/**
 * @file IAudioDecoder.hpp
 * @brief Interfaz común para los decodificadores de formatos de audio
 * @author DjSteker
 * @date 31 de agosto de 2026
 * @version 1.0
 */

#ifndef IAUDIODECODER_HPP_
#define IAUDIODECODER_HPP_

#include <string>
#include <cstddef>

/**
 * @class IAudioDecoder
 * @brief Interfaz que deben implementar todos los decodificadores de audio
 *
 * Desacopla a AudioEngine del formato concreto del archivo de audio.
 * Cada implementación (WAV, MP3, OGG...) sabe abrir su propio tipo de
 * archivo y entregar bloques de PCM (S16LE, entrelazado) listos para
 * enviar a ALSA. AudioEngine no necesita conocer el formato: solo pide
 * bloques de PCM al decoder activo.
 */
class IAudioDecoder {
public:
	/**
	 * @brief Destructor virtual
	 */
	virtual ~IAudioDecoder() = default;

	/**
	 * @brief Abre el archivo y prepara el decodificador para lectura
	 * @param filepath Ruta del archivo de audio a abrir
	 * @return true si el archivo se abrió y es válido para este decoder
	 */
	virtual bool open(const std::string &filepath) = 0;

	/**
	 * @brief Cierra el archivo y libera los recursos internos del decodificador
	 */
	virtual void close() = 0;

	/**
	 * @brief Rellena un buffer con PCM S16LE entrelazado decodificado
	 * @param buffer Puntero al buffer de destino
	 * @param buffer_size Tamaño máximo en bytes a escribir en el buffer
	 * @return Bytes realmente escritos en el buffer; 0 indica fin del stream
	 * o error de lectura
	 */
	virtual size_t readPcm(char *buffer, size_t buffer_size) = 0;

	/**
	 * @brief Obtiene la frecuencia de muestreo del audio decodificado
	 * @return Frecuencia de muestreo en Hz
	 */
	virtual int sampleRate() const = 0;

	/**
	 * @brief Obtiene el número de canales del audio decodificado
	 * @return Número de canales (1 = mono, 2 = estéreo)
	 */
	virtual int channels() const = 0;
};

#endif /* IAUDIODECODER_HPP_ */
