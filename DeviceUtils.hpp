/*
 * DeviceUtils.hpp
 *
 *  Created on: 25 jul 2026
 *      Author: usuario001
 */

/**
 * @file DeviceUtils.hpp
 * @brief Utilidades para detectar y gestionar dispositivos de audio
 * @author usuario001
 * @date 25 de julio de 2026
 * @version 1.0
 */

#ifndef DEVICEUTILS_HPP_
#define DEVICEUTILS_HPP_

#include <vector>
#include <string>
#include <mutex>

/**
 * @class DeviceUtils
 * @brief Singleton que proporciona acceso a dispositivos de audio disponibles
 *
 * Gestiona la detección y caché de dispositivos de audio ALSA disponibles.
 * Implementa el patrón Singleton para garantizar una única instancia.
 */
class DeviceUtils {
public:
	/**
	 * @brief Obtiene la instancia singleton de DeviceUtils
	 * @return Referencia a la instancia única de DeviceUtils
	 */
	static DeviceUtils& getInstance();

	/**
	 * @brief Obtiene la lista de dispositivos de reproducción disponibles
	 * @return Vector de nombres de dispositivos de reproducción
	 */
	std::vector<std::string> getPlaybackDevices();

	/**
	 * @brief Obtiene la lista de dispositivos PCM disponibles
	 * @return Vector de nombres de dispositivos PCM
	 */
	std::vector<std::string> getPcmDevices();

private:
	/**
	 * @brief Constructor privado para el patrón Singleton
	 */
	DeviceUtils();

	/**
	 * @brief Destructor privado para el patrón Singleton
	 */
	~DeviceUtils();

	/**
	 * @brief Constructor de copia eliminado (no copiable)
	 */
	DeviceUtils(const DeviceUtils&) = delete;

	/**
	 * @brief Operador de asignación eliminado (no asignable)
	 */
	DeviceUtils& operator=(const DeviceUtils&) = delete;

	/**
	 * @brief Caché de dispositivos de reproducción
	 */
	std::vector<std::string> cached_playback_devices_;

	/**
	 * @brief Caché de dispositivos PCM
	 */
	std::vector<std::string> cached_pcm_devices_;

	/**
	 * @brief Mutex para sincronización de caché
	 */
	std::mutex cache_mutex_;

	/**
	 * @brief Indica si el caché es válido
	 */
	bool cache_valid_ = false;

	/**
	 * @brief Actualiza el caché de dispositivos
	 */
	void updateCache();
};

#endif /* DEVICEUTILS_H_ */
