/*
 * SfmlAudioDecoder.hpp
 *
 *  Created on: 31 ago 2026
 *      Author: DjSteker
 */

/**
 * @file SfmlAudioDecoder.hpp
 * @brief Decodificador de audio genérico apoyado en el módulo Audio de SFML 3
 * @author DjSteker
 * @date 31 de agosto de 2026
 * @version 1.0
 */

#ifndef SFMLAUDIODECODER_HPP_
#define SFMLAUDIODECODER_HPP_

#include "IAudioDecoder.hpp"
#include <SFML/Audio/InputSoundFile.hpp>
#include <optional>

/**
 * @class SfmlAudioDecoder
 * @brief Decodificador que delega en sf::InputSoundFile (SFML 3, módulo Audio)
 *
 * sf::InputSoundFile detecta el formato del archivo por su contenido y
 * sabe decodificar, entre otros, OGG/Vorbis y MP3, entregando siempre
 * PCM entero de 16 bits entrelazado — el mismo formato que usa el
 * resto de decodificadores de esta app (WavDecoder incluido).
 *
 * Se usa como implementación compartida por Mp3Decoder y OggDecoder
 * mientras no se dispone de librerías propias para esos formatos: la
 * API de SFML no distingue entre ambos casos de uso.
 *
 * @note Requiere enlazar con sfml-audio (paquete sfml3-devel en
 * openSUSE Tumbleweed). SFML 3 sustituyó los métodos "open"/"load"
 * que devolvían bool por factorías estáticas que devuelven
 * std::optional, de ahí el uso de std::optional aquí para almacenar
 * la instancia de sf::InputSoundFile (no es copiable ni construible
 * por defecto sin un archivo ya abierto).
 */
class SfmlAudioDecoder: public IAudioDecoder {
public:
	/**
	 * @brief Constructor del decodificador
	 */
	SfmlAudioDecoder();

	/**
	 * @brief Destructor del decodificador
	 * Cierra el archivo si estuviera abierto
	 */
	~SfmlAudioDecoder() override;

	/**
	 * @brief Abre el archivo de audio y detecta su formato por contenido
	 * @param filepath Ruta del archivo a abrir (mp3, ogg, wav, flac...)
	 * @return true si SFML pudo abrir y reconocer el archivo
	 */
	bool open(const std::string &filepath) override;

	/**
	 * @brief Cierra el archivo abierto, liberando el sf::InputSoundFile interno
	 */
	void close() override;

	/**
	 * @brief Decodifica el siguiente bloque de PCM S16 entrelazado
	 * @param buffer Puntero al buffer de destino
	 * @param buffer_size Tamaño máximo en bytes a escribir
	 * @return Bytes realmente decodificados; 0 si no quedan datos
	 */
	size_t readPcm(char *buffer, size_t buffer_size) override;

	/**
	 * @brief Obtiene la frecuencia de muestreo del archivo abierto
	 * @return Frecuencia de muestreo en Hz
	 */
	int sampleRate() const override;

	/**
	 * @brief Obtiene el número de canales del archivo abierto
	 * @return Número de canales
	 */
	int channels() const override;

private:
	/**
	 * @brief Instancia de sf::InputSoundFile, envuelta en std::optional
	 * porque la clase no es construible por defecto ni copiable
	 */
	std::optional<sf::InputSoundFile> file_;

	/**
	 * @brief Frecuencia de muestreo negociada al abrir el archivo
	 */
	int sample_rate_;

	/**
	 * @brief Número de canales negociado al abrir el archivo
	 */
	int channels_;
};

#endif /* SFMLAUDIODECODER_HPP_ */
