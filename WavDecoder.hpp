/*
 * WavDecoder.hpp
 *
 *  Created on: 31 ago 2026
 *      Author: DjSteker
 */

/**
 * @file WavDecoder.hpp
 * @brief Decodificador de archivos WAV (PCM sin compresión)
 * @author DjSteker
 * @date 31 de agosto de 2026
 * @version 1.0
 */

#ifndef WAVDECODER_HPP_
#define WAVDECODER_HPP_

#include "IAudioDecoder.hpp"
#include <fstream>

/**
 * @class WavDecoder
 * @brief Decodificador para archivos WAV (formato PCM sin comprimir)
 *
 * Parsea la cabecera RIFF/WAVE para extraer frecuencia de muestreo,
 * número de canales y localización de los datos de audio, y a partir
 * de ahí sirve el PCM leyendo directamente del archivo, ya que el WAV
 * no requiere descompresión.
 */
class WavDecoder: public IAudioDecoder {
public:
	/**
	 * @brief Constructor del decodificador WAV
	 */
	WavDecoder();

	/**
	 * @brief Destructor del decodificador WAV
	 * Cierra el archivo si estuviera abierto
	 */
	~WavDecoder() override;

	/**
	 * @brief Abre el archivo WAV y parsea su cabecera
	 * @param filepath Ruta del archivo .wav a abrir
	 * @return true si la cabecera es válida (RIFF/WAVE, PCM)
	 */
	bool open(const std::string &filepath) override;

	/**
	 * @brief Cierra el archivo WAV abierto
	 */
	void close() override;

	/**
	 * @brief Lee PCM directamente de los datos de audio del archivo
	 * @param buffer Puntero al buffer de destino
	 * @param buffer_size Tamaño máximo en bytes a escribir
	 * @return Bytes realmente leídos; 0 si no quedan datos
	 */
	size_t readPcm(char *buffer, size_t buffer_size) override;

	/**
	 * @brief Obtiene la frecuencia de muestreo leída de la cabecera
	 * @return Frecuencia de muestreo en Hz
	 */
	int sampleRate() const override;

	/**
	 * @brief Obtiene el número de canales leído de la cabecera
	 * @return Número de canales
	 */
	int channels() const override;

private:
	/**
	 * @brief Archivo WAV abierto en modo binario
	 */
	std::ifstream file_;

	/**
	 * @brief Offset en bytes donde comienzan los datos de audio
	 */
	size_t data_offset_;

	/**
	 * @brief Tamaño total en bytes de los datos de audio
	 */
	size_t data_size_;

	/**
	 * @brief Bytes de datos de audio leídos hasta el momento
	 */
	size_t bytes_read_;

	/**
	 * @brief Frecuencia de muestreo en Hz
	 */
	int sample_rate_;

	/**
	 * @brief Número de canales (1 = mono, 2 = estéreo)
	 */
	int channels_;

	/**
	 * @brief Parsea la cabecera RIFF/WAVE del archivo abierto en file_
	 * @return true si la cabecera es válida y el formato es PCM (1)
	 */
	bool parseHeader();
};

#endif /* WAVDECODER_HPP_ */
