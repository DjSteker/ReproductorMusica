/*
 * DeviceUtils.cpp
 *
 *  Created on: 22 oct 2025
 *      Author: usuario001
 */

#include "DeviceUtils.hpp"
#include <alsa/asoundlib.h>
#include <cstring>
#include <algorithm>

DeviceUtils::DeviceUtils() :
		cache_valid_(false) {
}

DeviceUtils::~DeviceUtils() {
}

DeviceUtils& DeviceUtils::getInstance() {
	static DeviceUtils instance;
	return instance;
}

void DeviceUtils::updateCache() {
	std::lock_guard<std::mutex> lock(cache_mutex_);
	void **hints = nullptr;
	if (snd_device_name_hint(-1, "pcm", &hints) < 0) {
		cache_valid_ = false;
		return;
	}

	cached_playback_devices_.clear();
	cached_pcm_devices_.clear();

	void **n = hints;
	while (*n != nullptr) {
		char *name = snd_device_name_get_hint(*n, "NAME");
		char *io = snd_device_name_get_hint(*n, "IOID");

		if (name != nullptr) {
			if (io == nullptr || strcmp(io, "Output") == 0 || strcmp(io, "Playback") == 0) {
				cached_playback_devices_.push_back(name);
			}
			if (io == nullptr || strcmp(io, "Input") == 0 || strcmp(io, "Capture") == 0) {
				cached_pcm_devices_.push_back(name);
			}
		}

		if (name != nullptr)
			free(name);
		if (io != nullptr)
			free(io);
		n++;
	}
	snd_device_name_free_hint(hints);

	// Añadir "default" si no está presente
	bool has_default_playback = false;
	for (const auto &dev : cached_playback_devices_) {
		if (dev.find("default") != std::string::npos) {
			has_default_playback = true;
			break;
		}
	}
	if (!has_default_playback) {
		cached_playback_devices_.insert(cached_playback_devices_.begin(), "default");
	}

	cache_valid_ = true;
}

std::vector<std::string> DeviceUtils::getPlaybackDevices() {
	if (!cache_valid_) {
		updateCache();
	}
	std::lock_guard<std::mutex> lock(cache_mutex_);
	return cached_playback_devices_;
}

std::vector<std::string> DeviceUtils::getPcmDevices() {
	if (!cache_valid_) {
		updateCache();
	}
	std::lock_guard<std::mutex> lock(cache_mutex_);
	return cached_pcm_devices_;
}

// Funciones globales para compatibilidad con el código existente
std::vector<std::string> get_playback_devices() {
	return DeviceUtils::getInstance().getPlaybackDevices();
}

std::vector<std::string> get_pcm_devices() {
	return DeviceUtils::getInstance().getPcmDevices();
}
