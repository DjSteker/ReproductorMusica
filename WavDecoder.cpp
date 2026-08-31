/*
 * WavDecoder.cpp
 *
 *  Created on: 31 ago 2026
 *      Author: DjSteker
 */

#include "WavDecoder.hpp"
#include <cstring>
#include <algorithm>
#include <cstdint>

WavDecoder::WavDecoder() :
		data_offset_(0), data_size_(0), bytes_read_(0), sample_rate_(44100), channels_(2) {
}

WavDecoder::~WavDecoder() {
	close();
}

bool WavDecoder::parseHeader() {
	char header[44];
	if (!file_.read(header, 44)) {
		return false;
	}
	if (std::memcmp(header, "RIFF", 4) != 0 || std::memcmp(header + 8, "WAVE", 4) != 0) {
		return false;
	}

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

	// Solo se admite PCM sin comprimir (audio_format == 1)
	if (audio_format != 1) {
		return false;
	}

	data_size_ = data_size32;
	data_offset_ = 44;
	return true;
}

bool WavDecoder::open(const std::string &filepath) {
	close();

	file_.open(filepath, std::ios::binary);
	if (!file_.is_open()) {
		return false;
	}

	if (!parseHeader()) {
		file_.close();
		return false;
	}

	file_.seekg(data_offset_);
	bytes_read_ = 0;
	return true;
}

void WavDecoder::close() {
	if (file_.is_open()) {
		file_.close();
	}
	file_.clear();
	data_offset_ = 0;
	data_size_ = 0;
	bytes_read_ = 0;
}

size_t WavDecoder::readPcm(char *buffer, size_t buffer_size) {
	if (!file_.is_open()) {
		return 0;
	}

	size_t remaining = data_size_ - bytes_read_;
	size_t to_read = std::min(buffer_size, remaining);
	if (to_read == 0) {
		return 0;
	}

	if (!file_.read(buffer, to_read)) {
		return 0;
	}

	bytes_read_ += to_read;
	return to_read;
}

int WavDecoder::sampleRate() const {
	return sample_rate_;
}

int WavDecoder::channels() const {
	return channels_;
}
