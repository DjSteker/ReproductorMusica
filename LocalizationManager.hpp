/*
 * LocalizationManager.hpp
 *
 *  Created on: 25 jul 2026
 *      Author: usuario001
 */

/**
 * @file LocalizationManager.hpp
 * @brief Gestor avanzado de localización con soporte a rutas y traducciones por defecto
 * @author usuario001
 * @date 25 de julio de 2026
 * @version 1.0
 */

#ifndef LOCALIZATIONMANAGER_HPP_
#define LOCALIZATIONMANAGER_HPP_

#include <string>
#include <unordered_map>
#include <filesystem>

/**
 * @class LocalizationManager
 * @brief Gestor mejorado de localización con caché y traducciones por defecto
 *
 * Proporciona localización de interfaz con soporte a múltiples idiomas,
 * caché de traducciones y fallback a traducciones por defecto.
 */
class LocalizationManager {
public:
	/**
	 * @brief Constructor del gestor de localización
	 */
	LocalizationManager();

	/**
	 * @brief Destructor del gestor de localización
	 */
	~LocalizationManager();

	/**
	 * @brief Carga las traducciones para un idioma específico
	 * @param language Código del idioma (ej: "es" para español, "en" para inglés)
	 * @return true si la carga fue exitosa
	 */
	bool load(const std::string &language = "es");

	/**
	 * @brief Obtiene el texto traducido para una clave
	 * @param key Clave del texto a buscar
	 * @return Referencia al texto traducido, o la clave si no se encuentra
	 */
	const std::string& getText(const std::string &key) const;

	/**
	 * @brief Obtiene la ruta al archivo de localización actual
	 * @return Ruta del archivo XML de localización
	 */
	std::filesystem::path getLocalizationFilePath() const;

private:
	/**
	 * @brief Mapa de traducciones (clave -> valor)
	 */
	std::unordered_map<std::string, std::string> translations_;

	/**
	 * @brief Idioma actual cargado
	 */
	std::string current_language_;

	/**
	 * @brief Ruta al directorio de localización
	 */
	std::filesystem::path localization_path_;

	/**
	 * @brief Obtiene el directorio del ejecutable
	 * @return Ruta absoluta del directorio del ejecutable
	 */
	std::filesystem::path getExecutableDirectory() const;

	/**
	 * @brief Carga traducciones por defecto si falla la carga del archivo XML
	 * Proporciona fallback a textos en inglés
	 */
	void loadDefaultTranslations();
};

#endif /* LOCALIZATION_HPP_ */
