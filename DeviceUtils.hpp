/*
 * DeviceUtils.hpp
 *
 *  Created on: 25 jul 2026
 *      Author: usuario001
 */

#ifndef DEVICEUTILS_HPP_
#define DEVICEUTILS_HPP_

#include <vector>
#include <string>
#include <mutex>

class DeviceUtils {
public:
	static DeviceUtils& getInstance();
	std::vector<std::string> getPlaybackDevices();
	std::vector<std::string> getPcmDevices();

private:
	DeviceUtils();
	~DeviceUtils();
	DeviceUtils(const DeviceUtils&) = delete;
	DeviceUtils& operator=(const DeviceUtils&) = delete;

	std::vector<std::string> cached_playback_devices_;
	std::vector<std::string> cached_pcm_devices_;
	std::mutex cache_mutex_;
	bool cache_valid_ = false;

	void updateCache();
};

#endif /* DEVICEUTILS_H_ */
