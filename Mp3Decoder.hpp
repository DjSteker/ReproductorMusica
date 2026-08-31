/*
 * Mp3Decoder.hpp
 *
 *  Created on: 31 ago 2026
 *      Author: DjSteker
 */

/**
 * @file Mp3Decoder.hpp
 * @brief Decodificador de archivos MP3
 * @author DjSteker
 * @date 31 de agosto de 2026
 * @version 1.1
 */

#ifndef MP3DECODER_HPP_
#define MP3DECODER_HPP_

#include "SfmlAudioDecoder.hpp"

/**
 * @class Mp3Decoder
 * @brief Decodificador para archivos MP3
 *
 * Implementación temporal ("por el momento") apoyada en
 * SfmlAudioDecoder (SFML 3, módulo Audio). Se mantiene como clase
 * propia, en vez de usar SfmlAudioDecoder directamente en
 * AudioEngine, para poder sustituirla más adelante por una librería
 * MP3 específica sin tocar AudioEngine ni el resto del código: basta
 * con cambiar aquí de qué hereda o reescribir open()/readPcm() contra
 * la API de esa librería.
 */
class Mp3Decoder: public SfmlAudioDecoder {
};

#endif /* MP3DECODER_HPP_ */
