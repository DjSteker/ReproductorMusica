/*
 * Localization.hpp
 *
 *  Created on: 25 jul 2026
 *      Author: usuario001
 */

/**
 * @file Localization.hpp
 * @brief Gestor de localización e internacionalización
 * @author usuario001
 * @date 25 de julio de 2026
 * @version 1.0
 */

#ifndef LOCALIZATION_HPP_
#define LOCALIZATION_HPP_

#include <string>
#include <unordered_map>
#include <tinyxml2.h>

/**
 * @class Localization
 * @brief Proporciona funcionalidad de localización de textos
 *
 * Carga traducciones desde archivos XML y proporciona acceso a textos
 * localizados mediante claves. Soporta múltiples idiomas.
 */
class Localization {
public:
	/**
	 * @brief Constructor de la clase Localization
	 */
	Localization();

	/**
	 * @brief Carga las traducciones para un idioma específico
	 * @param language Código del idioma (ej: "es" para español, "en" para inglés)
	 * @return true si la carga fue exitosa, false en caso contrario
	 */
	bool load(const std::string &language = "es");

	/**
	 * @brief Obtiene el texto traducido para una clave específica
	 * @param key Clave del texto a buscar
	 * @note Devuelve por valor para evitar crashes por referencias a temporales destruidos
	 * @return Texto traducido, o la clave si no se encuentra
	 */
	std::string getText(const std::string &key) const;

private:
	/**
	 * @brief Mapa de traducciones (clave -> valor)
	 */
	std::unordered_map<std::string, std::string> translations_;

	/**
	 * @brief Idioma actual cargado
	 */
	std::string current_language_;
};

#endif /* LOCALIZATION_HPP_ */
