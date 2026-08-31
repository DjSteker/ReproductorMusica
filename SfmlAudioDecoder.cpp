/*
 * SfmlAudioDecoder.cpp
 *
 *  Created on: 31 ago 2026
 *      Author: DjSteker
 */

#include "SfmlAudioDecoder.hpp"
#include <cstdint>

SfmlAudioDecoder::SfmlAudioDecoder() :
		sample_rate_(44100), channels_(2) {
}

SfmlAudioDecoder::~SfmlAudioDecoder() {
	close();
}

bool SfmlAudioDecoder::open(const std::string &filepath) {
	close();

	file_.emplace();
	if (!file_->openFromFile(filepath)) {
		file_.reset();
		return false;
	}

	sample_rate_ = static_cast<int>(file_->getSampleRate());
	channels_ = static_cast<int>(file_->getChannelCount());
	return true;
}

void SfmlAudioDecoder::close() {
	// std::optional::reset() destruye el sf::InputSoundFile interno,
	// que a su vez cierra el archivo en su propio destructor.
	file_.reset();
}

size_t SfmlAudioDecoder::readPcm(char *buffer, size_t buffer_size) {
	if (!file_.has_value()) {
		return 0;
	}

	std::uint64_t max_samples = buffer_size / sizeof(std::int16_t);
	std::uint64_t read = file_->read(reinterpret_cast<std::int16_t*>(buffer), max_samples);
	return static_cast<size_t>(read) * sizeof(std::int16_t);
}

int SfmlAudioDecoder::sampleRate() const {
	return sample_rate_;
}

int SfmlAudioDecoder::channels() const {
	return channels_;
}
