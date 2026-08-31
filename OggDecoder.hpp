/*
 * OggDecoder.hpp
 *
 *  Created on: 31 ago 2026
 *      Author: DjSteker
 */

/**
 * @file OggDecoder.hpp
 * @brief Decodificador de archivos OGG/Vorbis
 * @author DjSteker
 * @date 31 de agosto de 2026
 * @version 1.1
 */

#ifndef OGGDECODER_HPP_
#define OGGDECODER_HPP_

#include "SfmlAudioDecoder.hpp"

/**
 * @class OggDecoder
 * @brief Decodificador para archivos OGG (contenedor con audio Vorbis)
 *
 * Implementación temporal ("por el momento") apoyada en
 * SfmlAudioDecoder (SFML 3, módulo Audio). Se mantiene como clase
 * propia, igual que Mp3Decoder, para poder sustituirla más adelante
 * sin tocar AudioEngine ni el resto del código.
 */
class OggDecoder: public SfmlAudioDecoder {
};

#endif /* OGGDECODER_HPP_ */
